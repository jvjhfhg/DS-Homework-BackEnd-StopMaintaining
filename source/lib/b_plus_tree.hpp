#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include<iostream>
#include<cmath>
#include<cstring>
#include"utility.hpp"
#include"vector.hpp"

namespace sjtu {
	template<class Key, class T, class Compare = std::less<Key> >
	class BPTree {
        typedef long long ll;
	public:
		friend class ticket_map;
		friend class order_user;
		const char* file;
		static const int K = 4096;

		//max1,max2记录的分别是过渡节点、叶子节点的最大空间，min1,min2分别为前者的一半;
		static const int max1 = 6; // 2000 * 8 / (sizeof(Key));
		static const int max2 = 6; // 2000 * 8 / (sizeof(T));
		static const int min1 = max1 / 2;
		static const int min2 = max2 / 2;
		static const int max0 = 6;

		//节点保存了当前节点的位置，父亲的位置，前驱、后继的位置，一些信息，大小，是否是叶子节点;
		//所有的数组都是0_based;
		class node {
		public:
			ll pos, father, prev, succ;
			Key key[max0];
			ll son[max0];
			T data[max0];
			int size;
			bool is_leaf;
		public:
			node() {
				pos = father = prev = succ = 0;
				size = 0;
				is_leaf = false;
			}
		};

		//保存了BPTree的根节点的位置，以及整个文件的最后位置，每次的插入都从最后开始，从4096开始插入;
		ll root;
		ll at_end;
		FILE* File;

		Compare cmp;

		//开文件，并且定位;
		void file_seek(ll s) {
			fseek(File, s, SEEK_SET);
		}

		//关文件;
		void file_close() {
			fclose(File);
		}

		//构造函数需传参文件指针，默认函数会事先构造一个root;
		BPTree(const char* f) :file(f) {
			File = fopen(f, "rb+");
			file = f;
			node rt;
			at_end = K;
			rt.pos = at_end;
			at_end += K;
			root = rt.pos;
			file_seek(rt.pos);
			fwrite((char*)(&rt), sizeof(rt), 1, File);
		}

		~BPTree() {
			fclose(File);
		}

		//删库跑路;
		void clear() {
			File = fopen(file, "w");
			fclose(File);
			node rt;
			rt.pos = at_end = K;
			at_end += K;
			root = rt.pos;
			file_open(file, rt.pos);
			fwrite((char*)(&rt), sizeof(rt), 1, File);
		}

		//在当前块中查找键值所在位置，若键值小于当前的第一个键值，则返回-1;
		int search(const Key& kk, node* ss) {
			//	if(cmp(kk , ss->key[0])){
			//		return -1;
			//  }
			for (int i = 0; i < ss->size; ++i) {
				if (cmp(kk, ss->key[i])) {
					return i;
				}
			}
			return ss->size;
		}

		//在所有块中查找键值所在位置，只会找到对应的块的位置，不能查找在头的;
		ll search_node(const Key& kk) {
			char* tmp[K];
			file_seek(root);
			fread(tmp, K, 1, File);
			node* st = (node*)tmp;
			if (cmp(kk, st->key[0]))return st->pos;
			while (!st->is_leaf) {
				int p = search(kk, st) - 1;
				file_seek(st->son[p]);
				fread(tmp, K, 1, File);
				st = (node*)tmp;
			}
			return st->pos;
		}

		//查找主函数;
		pair<T&, bool> query(const Key& kk) {
			char* tmp[K];
			ll p = search_node(kk);
			file_seek(p);
			fread(tmp, K, 1, File);
			node* now = (node*)tmp;
			int k = search(kk, now) - 1;
			if (k == -1)  return pair<T&, bool>(now->data[0], false);
			else {
				if (cmp(now->key[k], kk)) {
					return pair<T&, bool>(now->data[k], false);
				}
				else {
					return pair<T&, bool>(now->data[k], true);
				}
			}
		}

		//中序遍历
		void traverse(node* s, vector<pair<Key, T> > &a) {
			char* tmp[K];
			if (s->is_leaf) {
				for (int i = 0; i < s->size; ++i) {
					a.push_back(pair<Key, T>(s->key[i], s->data[i]));
				}
			}
			else {
				//	a.push_back(pair<Key, T>(-1，-1));
				for (int i = 0; i < s->size; ++i) {
					char*tmp2[K];
					file_seek(s->son[i]);
					fread(tmp2, K, 1, File);
					node* now = (node*)tmp2;
					traverse(now, a);
				}
				//	a.push_back(pair<Key, T>(-2， - 2 ));
			}
		}

		vector<pair<Key, T> > traverse() {
			char* tmp[K];
			file_seek(root);
			fread(tmp, K, 1, File);
			node* rt = (node*)tmp;
			vector<pair<Key, T> > s;
			traverse(rt, s);
			return s;
		}

		//将一个块中第几个元素及其之后的元素后移一个;
		void move_back(int k, node* ss) {
			for (int i = ss->size; i > k; --i) {
				if (ss->is_leaf) {
					ss->key[i] = ss->key[i - 1];
					ss->data[i] = ss->data[i - 1];
				}
				else {
					ss->key[i] = ss->key[i - 1];
					ss->son[i] = ss->son[i - 1];
				}
			}
			ss->size++;
		}

		//将一个块中第几个元素及其之后的元素前移一个;
		void move_front(int k, node* ss) {
			for (int i = k - 1; i < ss->size - 1; ++i) {
				if (ss->is_leaf) {
					ss->key[i] = ss->key[i + 1];
					ss->data[i] = ss->data[i + 1];
				}
				else {
					ss->key[i] = ss->key[i + 1];
					ss->son[i] = ss->son[i + 1];
				}
			}
			ss->size--;
		}

		//更新一个节点的父亲的键值，若相等则不作为，若不等则修改;
		void update_father(node* s) {
			if (s->pos == root)return;
			char* tmp[K];
			file_seek(s->father);
			fread(tmp, K, 1, File);
			node* fa = (node*)tmp;
			for (int i = 0; i < fa->size; ++i) {
				if (fa->son[i] == s->pos) {
					if (fa->key[i] == s->key[0])  return;
					else {
						fa->key[i] = s->key[0];
						file_seek(fa->pos);
						fwrite((char*)fa, K, 1, File);
						update_father(fa);
						return;
					}
				}
			}
		}

		//更新一个块的儿子的父亲;
		void update_son(node* s, int k) {
			if (s->is_leaf)  return;
			char* tmp[K];
			file_seek(s->son[k]);
			fread(tmp, K, 1, File);
			node* so = (node*)tmp;
			so->father = s->pos;
			file_seek(so->pos);
			fwrite((char*)so, K, 1, File);
		}

		//分裂新块;
		void divide(node* a, int p) {
			char*tmp[K];
			node new_node;
			new_node.pos = at_end;
			at_end += K;
			new_node.father = a->father;
			new_node.succ = a->succ;
			new_node.prev = a->pos;
			if (a->succ != 0) {
				file_seek(a->succ);
				fread(tmp, K, 1, File);
				node* su = (node*)tmp;
				su->prev = new_node.pos;
				file_seek(su->pos);
				fwrite((char*)su, K, 1, File);
			}
			a->succ = new_node.pos;
			for (int i = p; i < a->size; ++i)
				if (a->is_leaf) {
					new_node.key[i - p] = a->key[i];
					new_node.data[i - p] = a->data[i];
				}
				else {
					new_node.key[i - p] = a->key[i];
					new_node.son[i - p] = a->son[i];
					update_son(&new_node, i - p);
				}
				new_node.size = a->size - p;
				a->size = p;
				new_node.is_leaf = a->is_leaf;
				if (!(a->is_leaf)) {
					file_seek(new_node.son[0]);
					fread(tmp, K, 1, File);
					((node*)tmp)->prev = 0;
					file_seek(new_node.son[0]);
					fwrite(tmp, K, 1, File);
					file_seek(a->son[a->size - 1]);
					fread(tmp, K, 1, File);
					((node*)tmp)->succ = 0;
					file_seek(a->son[a->size - 1]);
					fwrite(tmp, K, 1, File);
				}
				file_seek(new_node.pos);
				fwrite((char*)(&new_node), sizeof(new_node), 1, File);
				file_seek( a->pos);
				fwrite((char*)a, K, 1, File);
				file_seek(new_node.father);
				fread(tmp, K, 1, File);
				node* fa = (node*)tmp;
				int k = search(new_node.key[0], fa);
				move_back(k, fa);
				fa->key[k] = new_node.key[0];
				fa->son[k] = new_node.pos;
				file_seek(fa->pos);
				fwrite((char*)fa, K, 1, File);
		}

		//插入主函数
		bool insert(const Key& kk, const T& dd) {
			char* tmp[K];
			ll nn;
			file_seek(root);
			fread(tmp, K, 1, File);
			node* rt = (node*)tmp;
			if (rt->size == 0) {
				node new_node;
				new_node.pos = at_end;
				at_end += K;
				new_node.father = root;
				new_node.key[0] = kk;
				new_node.data[0] = dd;
				new_node.size = 1;
				new_node.is_leaf = true;
				rt->key[0] = kk;
				rt->son[0] = new_node.pos;
				rt->size++;
				file_seek(new_node.pos);
				fwrite((char*)(&new_node), sizeof(new_node), 1, File);
				fseek(File, root, SEEK_SET);
				fwrite((char*)rt, K, 1, File);
				return true;
			}
			node* now;
			if (cmp(kk, rt->key[0])) {
				ll p = search_node(rt->key[0]);
				file_seek(p);
				fread(tmp, K, 1, File);
				now = (node*)tmp;
				move_back(0, now);
				now->key[0] = kk;
				now->data[0] = dd;
				update_father(now);
				file_seek(now->pos);
				fwrite((char*)now, K, 1, File);
			}
			else {
				ll p = search_node(kk);
				file_seek(p);
				fread(tmp, K, 1, File);
				now = (node*)tmp;
				int k = search(kk, now);
				if (!cmp(now->key[k - 1], kk) && !(cmp(kk, now->key[k - 1]))) {
					return false;
				}
				else {
					move_back(k, now);
					now->key[k] = kk;
					now->data[k] = dd;
					file_seek(now->pos);
					fwrite((char*)now, K, 1, File);
				}
			}
			while (now->pos != root) {
				if (now->is_leaf && now->size < max2)  break;
				if (!(now->is_leaf) && now->size < max1)  break;
				divide(now, max0 / 2);
				file_seek(now->father);
				fread(tmp, K, 1, File);
				node* fa = (node*)tmp;
				now = fa;
			}
			if (now->pos == root && now->size >= max1) {
				node new_root;
				new_root.pos = at_end;
				at_end += K;
				root = new_root.pos;
				new_root.key[0] = now->key[0];
				new_root.son[0] = now->pos;
				now->father = new_root.pos;
				new_root.size = 1;
				file_seek(new_root.pos);
				fwrite((char*)(&new_root), sizeof(new_root), 1, File);
				divide(now, max0 / 2);
			}
			return true;
		}

		//修改主函数;
		void modify(const Key& kk,const T& dd) {
			char* tmp[K];
			ll p = search_node(kk);
			file_seek(p);
			fread(tmp, K, 1, File);
			node* now = (node*)tmp;
			int k = search(kk, now);
			//if(now->key[k] != kk){int X = 1 / 0;}
			now->key[k - 1] = kk;
			now->data[k - 1] = dd;
			update_father(now);
			file_seek(now->pos);
			fwrite((char*)now, K, 1, File);
		}

		//向左兄弟借儿子，若前者有足够的返回true;
		bool borrow_left(node* a) {
			char* tmp[K];
			char* tmp2[K];
			file_seek(a->prev);
			fread(tmp, K, 1, File);
			node* pr = (node*)tmp;
			if (pr->is_leaf) {
				if (pr->size > max2 / 2) {
					pr->size -= 1;
					move_back(0, a);
					a->key[0] = pr->key[pr->size];
					a->data[0] = pr->data[pr->size];
					file_seek(a->pos);
					fwrite((char*)a, K, 1, File);
					file_seek(pr->pos);
					fwrite((char*)pr, K, 1, File);
					update_father(a);
					return true;
				}
				else {
					return false;
				}
			}
			else {
				if (pr->size > max1 / 2) {
					pr->size -= 1;
					move_back(0, a);
					a->key[0] = pr->key[pr->size];
					a->son[0] = pr->son[pr->size];
					file_seek(a->son[0]);
					fread(tmp2, K, 1, File);
					((node*)tmp2)->prev = 0;
					((node*)tmp2)->succ = a->son[1];
					((node*)tmp2)->father = a->pos;
					file_seek(a->son[0]);
					fwrite(tmp2, K, 1, File);
					file_seek(a->son[1]);
					fread(tmp2, K, 1, File);
					((node*)tmp2)->prev = a->son[0];
					file_seek(a->son[1]);
					fwrite(tmp2, K, 1, File);
					file_seek(pr->son[pr->size - 1]);
					fread(tmp2, K, 1, File);
					((node*)tmp2)->succ = 0;
					file_seek(pr->son[pr->size - 1]);
					fwrite(tmp2, K, 1, File);
					file_seek(a->pos);
					fwrite((char*)a, K, 1, File);
					file_seek(pr->pos);
					fwrite((char*)pr, K, 1, File);
					update_father(a);
					return true;
				}
				else {
					return false;
				}
			}
		}

		//向右子树借一个孩子;
		bool borrow_right(node* a) {
			char* tmp[K];
			char* tmp2[K];
			file_seek(a->succ);
			fread(tmp, K, 1, File);
			node* su = (node*)tmp;
			if (su->is_leaf) {
				if (su->size > max2 / 2) {
					a->key[a->size] = su->key[0];
					a->data[a->size] = su->data[0];
					a->size += 1;
					move_front(1, su);
					file_seek(su->pos);
					fwrite((char*)su, K, 1, File);
					file_seek(a->pos);
					fwrite(a, K, 1, File);
					update_father(su);
					return true;
				}
				else {
					return false;
				}
			}
			else {
				if (su->size > max1 / 2) {
					a->key[a->size] = su->key[0];
					a->son[a->size] = su->son[0];
					a->size += 1;
					move_front(1, su);
					file_seek(a->son[a->size - 1]);
					fread(tmp2, K, 1, File);
					((node*)tmp2)->prev = a->son[a->size - 2];
					((node*)tmp2)->succ = 0;
					((node*)tmp2)->father = a->pos;
					file_seek(a->son[a->size - 1]);
					fwrite(tmp2, K, 1, File);
					file_seek(a->son[a->size - 2]);
					fread(tmp2, K, 1, File);
					((node*)tmp2)->succ = a->son[a->size - 1];
					file_seek(a->son[a->size - 2]);
					fwrite(tmp2, K, 1, File);
					file_seek(su->son[0]);
					fread(tmp2, K, 1, File);
					((node*)tmp2)->prev = 0;
					file_seek(su->son[0]);
					fwrite(tmp2, K, 1, File);
					file_seek(su->pos);
					fwrite((char*)su, K, 1, File);
					file_seek(a->pos);
					fwrite(a, K, 1, File);
					update_father(su);
					return true;
				}
				else {
					return false;
				}
			}
		}

		//合并两个块;
		void merge_left(node* s) {
			char* tmp[K];
			char* tmp2[K];
			file_seek(s->prev);
			fread(tmp, K, 1, File);
			node* pr = (node*)tmp;
			if (s->is_leaf) {
				for (int i = 0; i < s->size; ++i) {
					pr->key[i + pr->size] = s->key[i];
					pr->data[i + pr->size] = s->data[i];
				}
				pr->size += s->size;
				if (s->succ != 0) {
					file_seek(s->succ);
					fread(tmp2, K, 1, File);
					node* susu = (node*)tmp2;
					susu->prev = pr->pos;
					file_seek(susu->pos);
					fwrite(tmp2, K, 1, File);
				}
				pr->succ = s->succ;
				file_seek(pr->pos);
				fwrite((char*)pr, K, 1, File);
				file_seek(pr->father);
				fread(tmp2, K, 1, File);
				node* fa = (node*)tmp2;
				int k = search(s->key[0], fa);
				move_front(k, fa);
				file_seek(fa->pos);
				fwrite((char*)fa, K, 1, File);
			}
			else {
				file_seek(pr->son[pr->size - 1]);
				fread(tmp2, K, 1, File);
				((node*)tmp2)->succ = s->son[0];
				file_seek(pr->son[pr->size - 1]);
				fwrite(tmp2, K, 1, File);
				file_seek(s->son[0]);
				fread(tmp2, K, 1, File);
				((node*)tmp2)->prev = pr->son[pr->size - 1];
				file_seek(s->son[0]);
				fwrite(tmp2, K, 1, File);
				for (int i = 0; i < s->size; ++i) {
					pr->key[i + pr->size] = s->key[i];
					pr->son[i + pr->size] = s->son[i];
					update_son(pr, pr->size + i);
				}
				pr->size += s->size;
				pr->succ = s->succ;
				if (s->succ != 0) {
					file_seek(s->succ);
					fread(tmp2, K, 1, File);
					node* susu = (node*)tmp2;
					susu->prev = pr->pos;
					file_seek(susu->pos);
					fwrite(tmp2, K, 1, File);
				}
				file_seek(pr->pos);
				fwrite((char*)pr, K, 1, File);
				file_seek(pr->father);
				fread(tmp2, K, 1, File);
				node* fa = (node*)tmp2;
				int k = search(s->key[0], fa);
				move_front(k, fa);
				file_seek(fa->pos);
				fwrite((char*)fa, K, 1, File);
			}
		}

		void merge_right(node* s) {
			char* tmp[K];
			char* tmp2[K];
			file_seek(s->succ);
			fread(tmp, K, 1, File);
			node* su = (node*)tmp;
			if (s->is_leaf) {
				for (int i = 0; i < su->size; ++i) {
					s->key[i + s->size] = su->key[i];
					s->data[i + s->size] = su->data[i];
				}
				s->size += su->size;
				s->succ = su->succ;
				if (su->succ != 0) {
					file_seek(su->succ);
					fread(tmp2, K, 1, File);
					node* susu = (node*)tmp2;
					susu->prev = s->pos;
					file_seek(susu->pos);
					fwrite(tmp2, K, 1, File);
				}
				file_seek(s->pos);
				fwrite((char*)s, K, 1, File);
				file_seek(s->father);
				fread(tmp2, K, 1, File);
				node* fa = (node*)tmp2;
				int k = search(su->key[0], fa);
				move_front(k, fa);
				file_seek(fa->pos);
				fwrite((char*)fa, K, 1, File);
			}
			else {
				file_seek(s->son[s->size - 1]);
				fread(tmp2, K, 1, File);
				((node*)tmp2)->succ = su->son[0];
				file_seek(s->son[s->size - 1]);
				fwrite(tmp2, K, 1, File);
				file_seek(su->son[0]);
				fread(tmp2, K, 1, File);
				((node*)tmp2)->prev = s->son[s->size - 1];
				file_seek(su->son[0]);
				fwrite(tmp2, K, 1, File);
				for (int i = 0; i < su->size; ++i) {
					s->key[i + s->size] = su->key[i];
					s->son[i + s->size] = su->son[i];
					update_son(s, s->size + i);
				}
				s->size += su->size;
				s->succ = su->succ;
				if (su->succ != 0) {
					file_seek(su->succ);
					fread(tmp2, K, 1, File);
					node* susu = (node*)tmp2;
					susu->prev = s->pos;
					file_seek(susu->pos);
					fwrite(tmp2, K, 1, File);
				}
				file_seek(s->pos);
				fwrite((char*)s, K, 1, File);
				file_seek(s->father);
				fread(tmp2, K, 1, File);
				node* fa = (node*)tmp2;
				int k = search(su->key[0], fa);
				move_front(k, fa);
				file_seek(fa->pos);
				fwrite((char*)fa, K, 1, File);
			}
		}

		//合并主函数;
		void merge(node* s) {
			if (s->pos == root) return;
			char* tmp[K];
			if (s->is_leaf) {
				if (s->size < max2 / 2) {
					if (s->prev != 0) {
						bool flag = borrow_left(s);
						if (!flag) {
							merge_left(s);
							file_seek(s->father);
							fread(tmp, K, 1, File);
							node*fa = (node*)tmp;
							merge(fa);
						}
						return;
					}
					if (s->succ != 0) {
						bool flag = borrow_right(s);
						if (!flag) {
							merge_right(s);
							file_seek(s->father);
							fread(tmp, K, 1, File);
							node*fa = (node*)tmp;
							merge(fa);
						}
						return;
					}
				}
				else  return;
			}
			else {
				if (s->size < max1 / 2) {
					if (s->prev != 0) {
						bool flag = borrow_left(s);
						if (!flag) {
							merge_left(s);
							file_seek(s->father);
							fread(tmp, K, 1, File);
							node*fa = (node*)tmp;
							merge(fa);
						}
						return;
					}
					if (s->succ != 0) {
						bool flag = borrow_right(s);
						if (!flag) {
							merge_right(s);
							file_seek(s->father);
							fread(tmp, K, 1, File);
							node*fa = (node*)tmp;
							merge(fa);
						}
						return;
					}
				}
				else  return;
			}
		}

		//删除主函数;
		void erase(const Key& kk) {
			char* tmp[K];
			char* tmp2[K];
			ll p = search_node(kk);
			file_seek(p);
			fread(tmp, K, 1, File);
			node* now = (node*)tmp;
			if (now->size == 1) {
				int cur = now->pos;
				while (now->pos != root && now->size == 1) {
					cur = now->pos;
					file_seek(now->father);
					fread(tmp, K, 1, File);
					now = (node*)tmp;
				}
				int k;
				for (k = 0; now->son[k] != cur; k++);
				if (k != 0) {
					file_seek(now->son[k - 1]);
					fread(tmp2, K, 1, File);
					node*pr = (node*)tmp2;
					pr->succ = k == now->size - 1 ? 0 : now->son[k + 1];
					file_seek(now->son[k - 1]);
					fwrite(tmp2, K, 1, File);
				}
				if (k != now->size - 1) {
					file_seek(now->son[k + 1]);
					fread(tmp2, K, 1, File);
					node*su = (node*)tmp2;
					su->prev = k == 0 ? 0 : now->son[k - 1];
					file_seek(now->son[k + 1]);
					fwrite(tmp2, K, 1, File);
				}
				move_front(k + 1, now);
				if (k == 0 && now->pos != root)
					update_father(now);
				file_seek(now->pos);
				fwrite((char*)now, K, 1, File);
				merge(now);
			}
			else {
				int k = search(kk, now);
				move_front(k, now);
				if (k == 1)update_father(now);
				file_seek(now->pos);
				fwrite((char*)now, K, 1, File);
				merge(now);
			}
		}
		bool check(ll cur) {
			char*tmp[K];
			char*tmp2[K];
			file_seek(cur);
			fread(tmp, K, 1, File);
			node*now = (node*)tmp;
			if (now->is_leaf)return 0;
			for (int i = 0; i < now->size; i++) {
				file_seek(now->son[i]);
				fread(tmp2, K, 1, File);
				node*s = (node*)tmp2;
				if (s->pos != now->son[i] ||
					s->prev != (i == 0 ? 0 : now->son[i - 1]) ||
					s->succ != (i == now->size - 1 ? 0 : now->son[i + 1]) ||
					s->father != now->pos || check(s->pos)) {
					return 1;
				}
			}
			return 0;
		}
	};
}
