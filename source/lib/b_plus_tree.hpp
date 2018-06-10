// #define _CRT_SECURE_NO_WARNINGS
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
		char _data[50];
        static const int K = 4096;

		//max1,max2?¡§???¡ã?£¤??a€¡é?¡ì??a€??£¤??a€ ?£¤?????|???¡¥?¡§??a€??|?????¡§? a€??¡ìa€??1?¡êa?????£¤?????£¤?-???¡§? a€??¡ìa€??1?¡ì??a€??|?¡°a???£¤?¡è?¡ì?¡ì???o??a€¡±?¡ä?¡¥???¡¯min1,min2?£¤??a€ ?£¤?????¡è???o?£¤a€¡ã???¡§a??a€|?¡ì??a€??¡è??a???£¤??? ;
		static const int max1 = 24; // 2000 * 8 / (sizeof(Key));
		static const int max2 = 24; // 2000 * 8 / (sizeof(T));
		static const int min1 = max1 / 2;
		static const int min2 = max2 / 2;
		static const int max0 = 24;

		//?¡§? a€??¡ìa€??1?¡è?????£¤?-???¡è?oa€ ?£¤??a€??£¤a€¡ã???¡§? a€??¡ìa€??1?¡ì??a€??¡è?????¡ì?????¡¥???¡¯?¡ì?????¡è?o?2?¡ì??a€??¡è?????¡ì?????¡¥???¡¯?£¤a€¡ã???????¡À?¡êa?????£¤?????¡ì???¡ì?¡ì??a€??¡è?????¡ì?????¡¥???¡¯?¡è??a???¡è?oa€o?¡è?????|???¡¥?¡¥???¡¯?£¤?¡è?¡ì?£¤?¡ã???¡¥???¡¯?|???¡¥?£¤???|?|???¡¥?£¤?????£¤?-???¡§? a€??¡ìa€??1;
		//?|a€¡ãa???|?¡°a€¡ã?¡ì??a€??|a€¡é?¡ã?¡ì??a€????¡¯???|???¡¥0_based;
		class node {
		public:
			ll pos, father, prev, succ;
			Key key[max0];
			ll son[max0];
			ll data[max0];
			int size;
			bool is_leaf;
		public:
			node() {
				pos = father = prev = succ = 0;
				size = 0;
				is_leaf = false;
			}
		};

		//?¡è?????£¤?-???¡è?oa€ BPtree?¡ì??a€??|? ?1?¡§? a€??¡ìa€??1?¡ì??a€??¡è?????¡ì?????¡¥???¡¯?¡è???£¤?£¤??? ?|a€¡é?¡ä?¡è???a?|a€¡°a€??¡è?????¡ì??a€??|?¡°a???£¤?????¡è?????¡ì?????¡¥???¡¯?|?¡¥???|?????¡ì??a€??|??a€??£¤a€|?£¤???¡¯???¡è?????|?¡°a???£¤?????£¤??a???£¤?¡ìa€1?¡¥???¡¯?¡è????4096?£¤??a???£¤?¡ìa€1?|??a€??£¤a€|?£¤;
		ll root;
		ll at_end;
		ll at_end2;
		FILE* File;
		FILE* Data;

		Compare cmp;

		//?£¤??a???|a€¡°a€??¡è?????¡¥???¡¯?£¤?1???¡è??a€??£¤?????¡è????;
		void file_seek(ll s) {
			fseek(File, s, SEEK_SET);
		}

		//?£¤a€|?3?|a€¡°a€??¡è????;
		void file_close() {
			fclose(File);
		}

		//?|??a€???a??? ?£¤a€????|a€¡é?¡ã???¡°a???¡è??? ?£¤??a€??|a€¡°a€??¡è?????|?¡¯a€???a€????¡¥???¡¯???????¡§???¡è?£¤a€????|a€¡é?¡ã?¡è?????¡è?oa€1?£¤a€|???|??a€???a??? ?¡è??a???¡è???aroot;
        BPTree(const char* f) :file(f) {
        	strcpy(_data , "data_of_");
        	strcat(_data+8 , f);
			if(!(File = fopen(file , "rb+"))){
				fclose(fopen(file , "w"));
				fclose(fopen(_data , "w"));
				File = fopen(file, "rb+");
				Data = fopen(_data, "rb+");
				node rt;
				at_end = K;
				rt.pos = at_end;
				at_end += K;
				at_end2 = sizeof(T);
				root = rt.pos;
				file_seek(rt.pos);
				fwrite((char*)(&rt), sizeof(rt), 1, File);
				file_seek(0);
				fwrite(&root , 8 , 1 ,File);
				file_seek(8);
				fwrite(&at_end , 8 , 1 ,File);
				file_seek(16);
				fwrite(&at_end2 , 8 , 1 ,File);
			}
			else{
				Data = fopen(_data, "rb+");
				file_seek(0);
				fread(&root , 8 , 1 ,File);
				file_seek(8);
				fread(&at_end , 8 , 1 ,File);
				file_seek(16);
				fread(&at_end2 , 8 , 1 ,File);
			}
		}

		~BPTree() {
			fclose(File);
			fclose(Data);
		}

		//?£¤??? ?£¤?oa€??¡§?¡¤a€??¡§?¡¤?¡¥;
		void clear() {
			fclose(File);
			fclose(Data);
			fclose(fopen(file, "w"));
			fclose(fopen(_data, "w"));
			node rt;
			rt.pos = at_end = K;
			at_end += K;
			at_end2 = sizeof(T);
			root = rt.pos;
			File = fopen(file, "rb+");
			Data = fopen(_data, "rb+");
			file_seek(rt.pos);
			fwrite((char*)(&rt), sizeof(rt), 1, File);
			file_seek(0);
			fwrite(&root , 8 , 1 ,File);
			file_seek(8);
			fwrite(&at_end , 8 , 1 ,File);
			file_seek(16);
			fwrite(&at_end2 , 8 , 1 ,File);
		}

		//?£¤?¡°?¡§?£¤??a€??£¤a€¡ã???£¤??a€¡±?¡è???-?|???£¤?|a€¡ã????a€????£¤a?????|a€¡ãa???£¤?¡°?¡§?¡è?????¡ì?????¡¥???¡¯?¡§a€1?£¤??a€????£¤a?????£¤?¡ã???¡è?o???£¤??a€??£¤a€¡ã???¡ì??a€??¡ì?????¡è??a???¡è???a??a€????£¤a?????¡¥???¡¯?£¤??a?¡é?¡§??a€??£¤a€o??-1;
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

		//?£¤?¡°?¡§?|a€¡ãa???|?¡°a€¡ã?£¤??a€¡±?¡è???-?|???£¤?|a€¡ã????a€????£¤a?????|a€¡ãa???£¤?¡°?¡§?¡è?????¡ì?????¡¥???¡¯?£¤???a?¡è?????|a€¡ã???£¤???¡ã?£¤?¡¥?1?£¤?oa€??¡ì??a€??£¤??a€¡±?¡ì??a€??¡è?????¡ì?????¡¥???¡¯?¡è?????¡§?¡¯???|???£¤?|a€¡ã???£¤?¡°?¡§?£¤?¡è?¡ä?¡ì??a€?;
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

		//?|???£¤?|a€¡ã???¡è?????£¤a€????|a€¡é?¡ã;
		pair<T, bool> query(const Key& kk) {
			char* tmp[K];
			if(at_end == 2 * K){
				T XX;
				return (pair<T, bool>){XX , false};
			}
			ll p = search_node(kk);
			file_seek(p);
			fread(tmp, K, 1, File);
			node* now = (node*)tmp;
			int k = search(kk, now) - 1;
			if (k == -1){
				T dd;
				fseek(Data, now->data[0], SEEK_SET);
				fread(&dd, sizeof(T), 1, Data);
				return pair<T, bool>(dd, false);
			}
			else {
				T dd;
				fseek(Data, now->data[k], SEEK_SET);
				fread(&dd, sizeof(T), 1, Data);
				if (cmp(now->key[k], kk)) {
					return pair<T, bool>(dd, false);
				}
				else {
					return pair<T, bool>(dd, true);
				}
			}
		}

		//?¡è???-?£¤?o?????????£¤??a€ 
		void traverse(node* s, vector<pair<Key, T> > &a) {
			char* tmp[K];
			if (s->is_leaf) {
				for (int i = 0; i < s->size; ++i) {
					T dd;
					fseek(Data, s->data[i], SEEK_SET);
					fread(&dd, sizeof(T), 1, Data);
					a.push_back(pair<Key, T>(s->key[i], dd));
				}
			}
			else {
				//	a.push_back(pair<Key, T>(-1?¡¥???¡¯-1));
				for (int i = 0; i < s->size; ++i) {
					char*tmp2[K];
					file_seek(s->son[i]);
					fread(tmp2, K, 1, File);
					node* now = (node*)tmp2;
					traverse(now, a);
				}
				//	a.push_back(pair<Key, T>(-2?¡¥???¡¯ - 2 ));
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

		//?£¤?¡ãa€ ?¡è??a???¡è???a?£¤??a€¡±?¡è???-?¡ì?????£¤a€?? ?¡è???a?£¤a€|?¡¯?¡ì?¡ä? ?£¤??? ?£¤a€|???¡è?1a€1?£¤?????¡ì??a€??£¤a€|?¡¯?¡ì?¡ä? ?£¤?????¡ì?¡ì???¡è??a???¡è???a;
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

		//?£¤?¡ãa€ ?¡è??a???¡è???a?£¤??a€¡±?¡è???-?¡ì?????£¤a€?? ?¡è???a?£¤a€|?¡¯?¡ì?¡ä? ?£¤??? ?£¤a€|???¡è?1a€1?£¤?????¡ì??a€??£¤a€|?¡¯?¡ì?¡ä? ?£¤a€¡ã???¡ì?¡ì???¡è??a???¡è???a;
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

		//?|a€o?¡ä?|a€¡°?¡ã?¡è??a???¡è???a?¡§? a€??¡ìa€??1?¡ì??a€??¡ì?????¡è?o?2?¡ì??a€???a€????£¤a?????¡¥???¡¯?¡§a€1?£¤?¡ìa€o???¡ì?-a€¡ã?£¤??a?¡é?¡è?????¡è???¡°?¡è???o?¡¥???¡¯?¡§a€1?£¤?¡è?????¡ì?-a€¡ã?£¤??a?¡é?¡è?????|a€??1;
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

		//?|a€o?¡ä?|a€¡°?¡ã?¡è??a???¡è???a?£¤??a€¡±?¡ì??a€??£¤a€????£¤?-???¡ì??a€??¡ì?????¡è?o?2;
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

		//?£¤??a€ ?¡§?¡êa€??|a€¡°?¡ã?£¤??a€¡±;
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

		//?|??a€??£¤a€|?£¤?¡è?????£¤a€????|a€¡é?¡ã
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
				new_node.data[0] = at_end2;
				at_end2 += sizeof(T);
				ll dt = new_node.data[0];
				fseek(Data, dt, SEEK_SET);
				fwrite(&dd, sizeof(T), 1, Data);
				new_node.size = 1;
				new_node.is_leaf = true;
				rt->key[0] = kk;
				rt->son[0] = new_node.pos;
				rt->size++;
				file_seek(new_node.pos);
				fwrite((char*)(&new_node), sizeof(new_node), 1, File);
				fseek(File, root, SEEK_SET);
				fwrite((char*)rt, K, 1, File);
				file_seek(0);
				fwrite(&root , 8 , 1 ,File);
				file_seek(8);
				fwrite(&at_end , 8 , 1 ,File);
				file_seek(16);
				fwrite(&at_end2 , 8 , 1 ,File);
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
				now->data[0] = at_end2;
				at_end2 += sizeof(T);
				ll dt = now->data[0];
				fseek(Data, dt, SEEK_SET);
				fwrite(&dd, sizeof(T), 1, Data);
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
					now->data[k] = at_end2;
					at_end2 += sizeof(T);
					ll dt = now->data[k];
					fseek(Data, dt, SEEK_SET);
					fwrite(&dd, sizeof(T), 1, Data);
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
			file_seek(0);
			fwrite(&root , 8 , 1 ,File);
			file_seek(8);
			fwrite(&at_end , 8 , 1 ,File);
			file_seek(16);
			fwrite(&at_end2 , 8 , 1 ,File);
			return true;
		}

		//?¡è?????|a€??1?¡è?????£¤a€????|a€¡é?¡ã;
		void modify(const Key& kk,const T& dd) {
			char* tmp[K];
			ll p = search_node(kk);
			file_seek(p);
			fread(tmp, K, 1, File);
			node* now = (node*)tmp;
			int k = search(kk, now);
			//if(now->key[k] != kk){int X = 1 / 0;}
			now->key[k - 1] = kk;
			ll dt = now->data[k - 1];
			fseek(Data, dt, SEEK_SET);
			fwrite(&dd, sizeof(T), 1, Data);
			if(k == 1)update_father(now);
			file_seek(now->pos);
			fwrite((char*)now, K, 1, File);
		}

		//?£¤??a€??£¤?¡¤?|?£¤a€|a€??£¤?????£¤a?????£¤a€????£¤?-???¡¥???¡¯?¡§a€1?£¤?£¤a€¡ã???¡§a??a€|?|?¡°a€¡ã?¡§???3?£¤?¡è???¡ì??a€??¡§??a€??£¤a€o??true;
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

		//?£¤??a€??£¤???3?£¤?-???|? a€??£¤a?????¡è??a???¡è???a?£¤?-???£¤?-??;
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

		//?£¤?????£¤?1???¡è???¡è?¡è???a?£¤??a€¡±;
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

		//?£¤?????£¤?1???¡è?????£¤a€????|a€¡é?¡ã;
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

		//?£¤??? ??a?¡é?¡è?¡è?????£¤a€????|a€¡é?¡ã;
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
			file_seek(0);
			fwrite(&root , 8 , 1 ,File);
			file_seek(8);
			fwrite(&at_end , 8 , 1 ,File);
			file_seek(16);
			fwrite(&at_end2 , 8 , 1 ,File);
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
