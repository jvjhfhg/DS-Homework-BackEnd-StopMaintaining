#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include<iostream>
#include<cmath>
#include<cstring>
#include<fstream>
#include"utility.hpp"
#include"vector.hpp"

namespace sjtu {
	template<class Key, class T, class Compare = std::less<Key> >
	class BPTree {
        typedef long long ll;
	public:
		const char* file;
		static const int K = 4096;

		//max1,max2è®°å½•çš„åˆ†åˆ«æ˜¯è¿‡æ¸¡èŠ‚ç‚¹ã€å¶å­èŠ‚ç‚¹çš„æœ€å¤§ç©ºé—´ï¼Œmin1,min2åˆ†åˆ«ä¸ºå‰è€…çš„ä¸€åŠ;
		static const int max1 = 16; // 2000 * 8 / (sizeof(Key));
		static const int max2 = 16; // 2000 * 8 / (sizeof(T));
		static const int min1 = max1 / 2;
		static const int min2 = max2 / 2;
		static const int max0 = 16;

		//èŠ‚ç‚¹ä¿å­˜äº†å½“å‰èŠ‚ç‚¹çš„ä½ç½®ï¼Œçˆ¶äº²çš„ä½ç½®ï¼Œå‰é©±ã€åŽç»§çš„ä½ç½®ï¼Œä¸€äº›ä¿¡æ¯ï¼Œå¤§å°ï¼Œæ˜¯å¦æ˜¯å¶å­èŠ‚ç‚¹;
		//æ‰€æœ‰çš„æ•°ç»„éƒ½æ˜¯0_based;
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

		//ä¿å­˜äº†BPtreeçš„æ ¹èŠ‚ç‚¹çš„ä½ç½®ï¼Œä»¥åŠæ•´ä¸ªæ–‡ä»¶çš„æœ€åŽä½ç½®ï¼Œæ¯æ¬¡çš„æ’å…¥éƒ½ä»Žæœ€åŽå¼€å§‹ï¼Œä»Ž4096å¼€å§‹æ’å…¥;
		ll root;
		ll at_end;
		FILE* File;

		Compare cmp;

		//å¼€æ–‡ä»¶ï¼Œå¹¶ä¸”å®šä½;
		void file_seek(ll s) {
			fseek(File, s, SEEK_SET);
		}

		//å…³æ–‡ä»¶;
		void file_close() {
			fclose(File);
		}

		//æž„é€ å‡½æ•°éœ€ä¼ å‚æ–‡ä»¶æŒ‡é’ˆï¼Œé»˜è®¤å‡½æ•°ä¼šäº‹å…ˆæž„é€ ä¸€ä¸ªroot;
        BPTree(const char* f) :file(f) {
			fclose(fopen(f , "w"));
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

		//åˆ åº“è·‘è·¯;
		void clear() {
			fclose(fopen(file, "w"));
			node rt;
			rt.pos = at_end = K;
			at_end += K;
			root = rt.pos;
			File = fopen(file, "rb+");
			file_seek(rt.pos);
			fwrite((char*)(&rt), sizeof(rt), 1, File);
		}

		//åœ¨å½“å‰å—ä¸­æŸ¥æ‰¾é”®å€¼æ‰€åœ¨ä½ç½®ï¼Œè‹¥é”®å€¼å°äºŽå½“å‰çš„ç¬¬ä¸€ä¸ªé”®å€¼ï¼Œåˆ™è¿”å›ž-1;
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

		//åœ¨æ‰€æœ‰å—ä¸­æŸ¥æ‰¾é”®å€¼æ‰€åœ¨ä½ç½®ï¼Œåªä¼šæ‰¾åˆ°å¯¹åº”çš„å—çš„ä½ç½®ï¼Œä¸èƒ½æŸ¥æ‰¾åœ¨å¤´çš„;
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

		//æŸ¥æ‰¾ä¸»å‡½æ•°;
		pair<T, bool> query(const Key& kk) {
			char* tmp[K];
			ll p = search_node(kk);
			file_seek(p);
			fread(tmp, K, 1, File);
			node* now = (node*)tmp;
			int k = search(kk, now) - 1;
			if (k == -1)  return pair<T, bool>(now->data[0], false);
			else {
				if (cmp(now->key[k], kk)) {
					return pair<T, bool>(now->data[k], false);
				}
				else {
					return pair<T, bool>(now->data[k], true);
				}
			}
		}

		//ä¸­åºéåŽ†
		void traverse(node* s, vector<pair<Key, T> > &a) {
			char* tmp[K];
			if (s->is_leaf) {
				for (int i = 0; i < s->size; ++i) {
					a.push_back(pair<Key, T>(s->key[i], s->data[i]));
				}
			}
			else {
				//	a.push_back(pair<Key, T>(-1ï¼Œ-1));
				for (int i = 0; i < s->size; ++i) {
					char*tmp2[K];
					file_seek(s->son[i]);
					fread(tmp2, K, 1, File);
					node* now = (node*)tmp2;
					traverse(now, a);
				}
				//	a.push_back(pair<Key, T>(-2ï¼Œ - 2 ));
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

		//å°†ä¸€ä¸ªå—ä¸­ç¬¬å‡ ä¸ªå…ƒç´ åŠå…¶ä¹‹åŽçš„å…ƒç´ åŽç§»ä¸€ä¸ª;
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

		//å°†ä¸€ä¸ªå—ä¸­ç¬¬å‡ ä¸ªå…ƒç´ åŠå…¶ä¹‹åŽçš„å…ƒç´ å‰ç§»ä¸€ä¸ª;
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

		//æ›´æ–°ä¸€ä¸ªèŠ‚ç‚¹çš„çˆ¶äº²çš„é”®å€¼ï¼Œè‹¥ç›¸ç­‰åˆ™ä¸ä½œä¸ºï¼Œè‹¥ä¸ç­‰åˆ™ä¿®æ”¹;
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

		//æ›´æ–°ä¸€ä¸ªå—çš„å„¿å­çš„çˆ¶äº²;
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

		//åˆ†è£‚æ–°å—;
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

		//æ’å…¥ä¸»å‡½æ•°
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

		//ä¿®æ”¹ä¸»å‡½æ•°;
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
			if(k == 1)update_father(now);
			file_seek(now->pos);
			fwrite((char*)now, K, 1, File);
		}

		//å‘å·¦å…„å¼Ÿå€Ÿå„¿å­ï¼Œè‹¥å‰è€…æœ‰è¶³å¤Ÿçš„è¿”å›žtrue;
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

		//å‘å³å­æ ‘å€Ÿä¸€ä¸ªå­©å­;
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

		//åˆå¹¶ä¸¤ä¸ªå—;
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

		//åˆå¹¶ä¸»å‡½æ•°;
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

		//åˆ é™¤ä¸»å‡½æ•°;
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
