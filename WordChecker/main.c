//
//  main.c
//  WordChecker
//
//  Created by Fabrizio Sordetti on 09/07/22.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct node{
    bool flag;
    struct node* p;
    struct node* left;
    struct node* right;
    struct node* next;
    char color;
    char key[];
} node_t;

typedef node_t* Ptr_node;

typedef struct tree{
    Ptr_node root;
    Ptr_node nil;
}tree_t;

typedef tree_t* Ptr_tree;

typedef struct filter{
    int inside;
    int min;
    bool count;
    bool* denyed;
}filter_t;

typedef filter_t* Ptr_filter;

void Inorder_tree_walk(Ptr_tree T, Ptr_node x);
Ptr_node Tree_search(Ptr_tree T, Ptr_node x, char* k);
Ptr_node Tree_minimum(Ptr_tree T, Ptr_node x);
Ptr_node Tree_maximum(Ptr_tree T, Ptr_node x);
Ptr_node Tree_successor(Ptr_tree T, Ptr_node x);
Ptr_node Tree_predecessor(Ptr_tree T, Ptr_node x);
void Left_rotate(Ptr_tree T, Ptr_node x);
void Right_rotate(Ptr_tree T, Ptr_node x);
void Tree_insert(Ptr_tree T, Ptr_node z);
void Tree_insert_fixup(Ptr_tree T, Ptr_node z);
Ptr_node Tree_delete(Ptr_tree T, Ptr_node z);
void Tree_delete_fixup(Ptr_tree T, Ptr_node x);
Ptr_tree nuova_partita(register unsigned int l, Ptr_tree allowedTree, Ptr_node* curr, Ptr_filter* list);
bool compare(register unsigned int len, char* refer, char* word, char* result);
Ptr_tree addAllowed(register unsigned int len, Ptr_tree allowedTree, Ptr_node* curr, Ptr_filter* list, char* needed, bool in);
unsigned int filtering(register unsigned int len, char* result, char* word, Ptr_tree allowedTree, Ptr_node* curr, Ptr_node* limit, Ptr_filter* list, char* needed, register unsigned int n_filtered);
unsigned int filtering_helper(register unsigned int len, Ptr_tree T, Ptr_node* curr, Ptr_node* limit, Ptr_filter* list, char* needed);
void print_filter(Ptr_tree T);
void reset_filter(Ptr_tree T, Ptr_node limit);
int stringcompare(char* str1, char* str2);

unsigned int acceptable[] ={0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 50, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77};

int main(void){
    Ptr_tree allowedTree=NULL;
    register unsigned int i, j, check, len;
    Ptr_filter list[78];
    Ptr_node curr=NULL;
    Ptr_node nodo;
    
    check = getchar_unlocked();
    len = 0;
    while(check!='\n'){
        len = (len * 10) + (check - '0');
        check = getchar_unlocked();
    }
    
    list[0]=(Ptr_filter)malloc(64*(sizeof(filter_t) + sizeof(bool)*len));
    
    list[0]->denyed=(bool*)(((unsigned char*)list[0])+ sizeof(filter_t));
    list[0]->inside=1;
    list[0]->min=0;
    list[0]->count=0;
    memset(list[0]->denyed, 0, len*sizeof(bool));
    
    for(i=1; i<64; i++){
        j=acceptable[i];
        list[j]=(Ptr_filter)(((unsigned char*)list[acceptable[i-1]]->denyed) + sizeof(bool)*len);
        list[j]->inside=1;
        list[j]->min=0;
        list[j]->count=0;
        list[j]->denyed=(bool*)(((unsigned char*)list[j]) + sizeof(filter_t));
        memset(list[j]->denyed, 0, len*sizeof(bool));
    }
    
    allowedTree=(Ptr_tree)malloc(sizeof(tree_t)+ sizeof(node_t));
    allowedTree->nil=(Ptr_node)(((unsigned char*) allowedTree) + sizeof(tree_t));
    allowedTree->nil->p=allowedTree->nil;
    allowedTree->nil->left=allowedTree->nil;
    allowedTree->nil->right=allowedTree->nil;
    allowedTree->nil->color='b';
    allowedTree->root=allowedTree->nil;
    
    curr=allowedTree->nil;
    
    check=getchar_unlocked();
    while(check!='+'){
        nodo=(Ptr_node)malloc(sizeof(node_t) + sizeof(char) * (len+1));
        i=0;
        while(check!='\n'){
            nodo->key[i]=check;
            check=getchar_unlocked();
            i++;
        }
        nodo->key[i]='\0';
        nodo->flag=1;
        nodo->p=allowedTree->nil;
        nodo->left=allowedTree->nil;
        nodo->right=allowedTree->nil;
        nodo->next=NULL;
        curr->next=nodo;
        curr=nodo;
        
        Tree_insert(allowedTree, nodo);
        
        check=getchar_unlocked();
    }
    
    check='+';
    while(check!=EOF){
        check=getchar_unlocked();
        switch (check) {
            case 'n':
                while(check!='\n'){
                    check=getchar_unlocked();
                }
                allowedTree=nuova_partita(len, allowedTree, &curr, list);
                break;
                
            case 'i':
                while(check!='\n'){
                    check=getchar_unlocked();
                }
                allowedTree=addAllowed(len, allowedTree, &curr, NULL, "", 0);
                break;
            default:
                break;
        }
    }
    
    return 0;
}

Ptr_tree nuova_partita(register unsigned int len, Ptr_tree allowedTree, Ptr_node* curr, Ptr_filter* list){
    
    register unsigned int n, check, i, j, n_filtered=0;
    bool win=0;
    char refer[len+1], word[len+1], result[len+1], needed[len+1];
    Ptr_node limit=allowedTree->nil;
    
    memset(needed, '/', len*sizeof(char));
    needed[len]='\0';
    
    check=getchar_unlocked();
    i=0;
    while(check!='\n'){
        refer[i]=check;
        check=getchar_unlocked();
        i++;
    }
    refer[i]='\0';
    
    check = getchar_unlocked();
    n = 0;
    while(check!='\n'){
        n = (n * 10) + (check - '0');
        check = getchar_unlocked();
    }
    
    while(n!=0 && win==0){
        check=getchar_unlocked();
        
        if(check!='+'){
            i=0;
            while(check!='\n'){
                word[i]=check;
                check=getchar_unlocked();
                i++;
            }
            word[i]='\0';
            
            if(Tree_search(allowedTree, allowedTree->root, word)!=allowedTree->nil){
                win = compare(len, refer, word, result);
                if(win==0){
                    n_filtered=filtering(len, result, word, allowedTree, curr, &limit, list, needed, n_filtered);
                    fprintf(stdout, "%d\n", n_filtered);
                }
                n--;
            }
            else{
                puts("not_exists");
            }
        }
        else{
            check=getchar_unlocked();
            switch (check) {
                case 's':
                    while(check!='\n'){
                        check=getchar_unlocked();
                    }
                    print_filter(allowedTree);
                    break;
                case 'i':
                    while(check!='\n'){
                        check=getchar_unlocked();
                    }
                    allowedTree=addAllowed(len, allowedTree, curr, list, needed, 1);
                    break;
                default:
                    break;
            }
        }
        
    }
    if(win!=0){
        puts("ok");
    } else{
        puts("ko");
    }
    
    reset_filter(allowedTree, limit);
    
    for(i=64; i--; ){
        j=acceptable[i];
        memset(list[j]->denyed, 0, len*sizeof(bool));
        list[j]->inside=1;
        list[j]->min=0;
        list[j]->count=0;
    }
    
    return allowedTree;
}

bool compare(register unsigned int len, char* refer, char* word, char* result){
    
    register unsigned int i, c;
    unsigned int list[78];
    
    if(stringcompare(refer, word)!=0){
        memset(list, 0, 78*sizeof(int));
        
        for(i=len; i--; ){
            if(word[i]!=refer[i]){
                list[(int)refer[i]-45]++;
            }
        }
        
        for(i=0; i<len; i++){
            c=(int)word[i]-45;
            if(refer[i]==word[i]){
                result[i]='+';
            }else{
                if(list[c]!=0){
                    result[i]='|';
                    list[c]--;
                } else{
                    result[i]='/';
                }
            }
            
        }
        result[i]='\0';
        puts(result);
        return 0;
    }
    else{
        return 1;
    }
}

Ptr_tree addAllowed(register unsigned int len, Ptr_tree allowedTree, Ptr_node* curr, Ptr_filter* list, char* needed, bool in){
    
    register unsigned int check, i, j, c;
    unsigned int n[78];
    Ptr_node nodo;
    Ptr_filter l;
    
    check=getchar_unlocked();
    while(check!='+'){
        
        nodo=(Ptr_node)malloc(sizeof(node_t) + sizeof(char) * (len+1));
        i=0;
        while(check!='\n'){
            nodo->key[i]=check;
            check=getchar_unlocked();
            i++;
        }
        nodo->key[i]='\0';
        nodo->flag=1;
        nodo->p=allowedTree->nil;
        nodo->left=allowedTree->nil;
        nodo->right=allowedTree->nil;
        nodo->next=NULL;
        
        if(in!=0){
            
            memset(n, 0, 78*sizeof(int));
            
            for(i=len; i--; ){
                c=(nodo->key[i])-45;
                l=list[c];
                if(l->inside==0 || (l->denyed)[i]==1 || (needed[i]!='/' && needed[i]!=(char)(c+45))){
                    nodo->flag=0;
                    break;
                }
                else{
                    n[c]++;
                }
            }
            if(nodo->flag!=0){
                for(i=64; i--; ){
                    j=acceptable[i];
                    l=list[j];
                    if(l->count) {
                        if(n[j]!=(l->min)){
                            nodo->flag=0;
                            break;
                        }
                    }else{
                       if(n[j]<l->min){
                           nodo->flag=0;
                           break;
                       }
                    }
                }
            }
            
        }
        
        
        if(nodo->flag!=0){
            (*curr)->next=nodo;
            *curr=nodo;
        } else{
            nodo->next=allowedTree->nil->next;
            allowedTree->nil->next=nodo;
        }
        Tree_insert(allowedTree, nodo);
        
        check=getchar_unlocked();
    }
    while(check!='\n'){
        check=getchar_unlocked();
    }
    
    return allowedTree;
}

unsigned int filtering(register unsigned int len, char* result, char* word, Ptr_tree allowedTree, Ptr_node* curr, Ptr_node* limit, Ptr_filter* list, char* needed,register unsigned int n_filtered){
    unsigned int min[78];
    register unsigned int i, flag=0, c;
    Ptr_filter l;
    
    if((*limit)->next->next==NULL){
        return n_filtered;
    }
    
    memset(min, 0, 78*sizeof(int));
    
    for(i=len; i--; ){
        c=word[i]-45;
        l=list[c];
        
        switch (result[i]) {
            case '+':
                if(l->inside!=2 || (l->denyed)[i]!=0 || needed[i]!=word[i]){
                    flag=1;
                }
                l->inside=2;
                needed[i]=word[i];
                (l->denyed)[i]=0;
                min[c]++;
                break;
                
            case '|':
                if(l->inside!=2 || (l->denyed)[i]!=1){
                    flag=1;
                }
                l->inside=2;
                (l->denyed)[i]=1;
                min[c]++;
                break;
                
            default:
                if(l->inside!=2){
                    if(l->inside!=0){
                        flag=1;
                    }
                    l->inside=0;
                }
                if(l->count!=1 || (l->denyed)[i]!=1){
                    flag=1;
                }
                l->count=1;
                (l->denyed)[i]=1;
                break;
        }
        
        if((l->min)<min[c]){
            
            flag=1;
            l->min=min[c];
        }
    }
    
    if(flag!=0){
        return filtering_helper(len, allowedTree, curr, limit, list, needed);
    }
    else{
        return n_filtered;
    }
}

unsigned int filtering_helper(register unsigned int len, Ptr_tree T, Ptr_node* curr, Ptr_node* limit, Ptr_filter* list, char* needed){
    bool flag;
    unsigned int j, n[78];
    register unsigned int i, c, count=0;
    char need;
    Ptr_node x, y, prec;
    Ptr_filter l;
    
    prec=*limit;
    x=(*limit)->next;
    
    while (x!=NULL){
        flag=1;
        
        memset(n, 0, 78*sizeof(int));
        
        for(i=len; i--; ){
            c=(x->key)[i]-45;
            l=list[c];
            need=needed[i];
            
            if(l->inside==0 || (l->denyed)[i]==1 || (need!='/' && need!=(char)(c+45))){
                flag=0;
                break;
            }
            else{
                n[c]++;
            }
        }
        if(flag!=0){
            for(i=64; i--; ){
                j=acceptable[i];
                l=list[j];
                if(l->count) {
                    if(n[j]!=(l->min)){
                        flag=0;
                        break;
                    }
                }else{
                    if(n[j]<(l->min)) {
                        flag=0;
                        break;
                    }
                }
            }
        }
        
        if(flag!=0){
            count++;
            prec=x;
            x=x->next;
        }
        else{
            x->flag=0;
            if(x==*curr){
                *curr=prec;
            }
            
            if(prec!=*limit){
                prec->next=x->next;
                y=x;
                x=x->next;
                y->next=(*limit)->next;
                (*limit)->next=y;
                *limit=y;
            }
            else{
                prec=x;
                x=x->next;
                *limit=prec;
            }
        }
        
    }
    
    return count;
}

void print_filter(Ptr_tree T){
    Ptr_node x=Tree_minimum(T, T->root);
    
    while(x!=T->nil){
        if(x->flag!=0) puts(x->key);
        x=Tree_successor(T, x);
    }
}

void reset_filter(Ptr_tree T, Ptr_node limit){
    Ptr_node x=T->nil;
    
    while(x!=limit){
        x->flag=1;
        x=x->next;
    }
    x->flag=1;
}

void Inorder_tree_walk(Ptr_tree T, Ptr_node x){
    if (x!=T->nil){
        Inorder_tree_walk(T, x->left);
        puts(x->key);
        Inorder_tree_walk(T, x->right);
    }
}

Ptr_node Tree_search(Ptr_tree T, Ptr_node x, char* k){
    int flag;
    if (x==T->nil){
        return x;
    }
    
    flag=stringcompare(k, x->key);
    if(flag==0){
        return x;
    }
    if (flag<0){
        return Tree_search(T, x->left, k);
    } else {
        return Tree_search(T, x->right, k);
    }
}

Ptr_node Tree_minimum(Ptr_tree T, Ptr_node x){
    while (x->left!=T->nil){
        x=x->left;
    }
    return x;
}

Ptr_node Tree_maximum(Ptr_tree T, Ptr_node x){
    while (x->right!=T->nil){
        x=x->right;
    }
    return x;
}

Ptr_node Tree_successor(Ptr_tree T, Ptr_node x){
    Ptr_node y;
    if (x->right!=T->nil){
        return Tree_minimum(T, x->right);
    }
    y=x->p;
    while (y!=T->nil && x==y->right){
        x=y;
        y=y->p;
    }
    return y;
}

Ptr_node Tree_predecessor(Ptr_tree T, Ptr_node x){
    Ptr_node y;
    if(x->left!=T->nil){
        return Tree_maximum(T, x->left);
    }
    y=x->p;
    while (y!=T->nil && x==y->left){
        x=y;
        y=y->p;
    }
    return y;
}

void Left_rotate(Ptr_tree T, Ptr_node x){
    Ptr_node y;
    y=x->right;
    x->right=y->left;
    
    if(y->left!=T->nil){
        y->left->p=x;
    }
    y->p = x->p;
    
    if(x->p==T->nil){
        T->root=y;
    } else if(x==x->p->left){
        x->p->left=y;
    } else{
        x->p->right=y;
    }
    y->left=x;
    x->p=y;
}

void Right_rotate(Ptr_tree T, Ptr_node x){
    Ptr_node y;
    y=x->left;
    x->left=y->right;
    
    if(y->right!=T->nil){
        y->right->p=x;
    }
    y->p = x->p;
    
    if(x->p==T->nil){
        T->root=y;
    } else if(x==x->p->left){
        x->p->left=y;
    } else{
        x->p->right=y;
    }
    y->right=x;
    x->p=y;
}

void Tree_insert(Ptr_tree T, Ptr_node z){
    Ptr_node y=T->nil;
    Ptr_node x=T->root;
    while (x!=T->nil){
        y=x;
        if (stringcompare(z->key, x->key)<0){
            x=x->left;
        }else{
            x=x->right;
        }
    }
    z->p=y;
    if(y==T->nil){
        T->root=z; //l'albero T è vuoto
        
    }
    else if (stringcompare(z->key, y->key)<0){
        y->left=z;
    }else{
        y->right=z;
    }
    
    z->left=T->nil;
    z->right=T->nil;
    z->color='r';
    Tree_insert_fixup(T,z);
}

void Tree_insert_fixup(Ptr_tree T, Ptr_node z){
    Ptr_node x, y;
    if(z==T->root){
        T->root->color='b';
    }
    else{
        x=z->p;
        if(x->color=='r'){
            if(x==x->p->left){
                y=x->p->right;
                if(y->color=='r'){
                    x->color='b';
                    y->color='b';
                    x->p->color='r';
                    Tree_insert_fixup(T, x->p);
                }
                else{
                    if(z==x->right){
                        z=x;
                        Left_rotate(T, z);
                        x=z->p;
                    }
                    x->color='b';
                    x->p->color='r';
                    Right_rotate(T, x->p);
                }
            }
            else{
                y=x->p->left;
                if(y->color=='r'){
                    x->color='b';
                    y->color='b';
                    x->p->color='r';
                    Tree_insert_fixup(T, x->p);
                }
                else{
                    if(z==x->left){
                        z=x;
                        Right_rotate(T, z);
                        x=z->p;
                    }
                    x->color='b';
                    x->p->color='r';
                    Left_rotate(T, x->p);
                }
            }
        }
    }
    
    
}


Ptr_node Tree_delete(Ptr_tree T, Ptr_node z){
    Ptr_node x, y;
    if (z->left==T->nil || z->right==T->nil){
        y=z;
    }else{
        y=Tree_successor(T, z);
    }
    if (y->left!=T->nil){
        x=y->left;
    }else{
        x=y->right;
    }
    x->p=y->p;
    if (y->p==T->nil){
        T->root=x;
    } else if (y==y->p->left){
        y->p->left=x;
    }
    else {
        y->p->right=x;
    }
    if (y!=z){
        strcpy(z->key, y->key);
    }
    if (y->color=='b'){
        Tree_delete_fixup(T,x);
    }
    return y;
}

void Tree_delete_fixup(Ptr_tree T, Ptr_node x){
    Ptr_node w;
    
    if(x->color=='r' || x->p==T->nil){
        x->color='b';
    }else{
        if(x==x->p->left){
            w=x->p->right;
            if(w->color=='r'){
                w->color='b';
                x->p->color='r';
                Left_rotate(T, x->p);
                w=x->p->right;
            }
            if(w->left->color=='b' && w->right->color=='b'){
                w->color='r';
                Tree_delete_fixup(T, x->p);
            }else {
                if(w->right->color=='b'){
                    w->left->color='b';
                    w->color='r';
                    Right_rotate(T, w);
                    w=x->p->right;
                }
                w->color=x->p->color;
                x->p->color='b';
                Left_rotate(T, x->p);
            }
        } else{
            w=x->p->left;
            if(w->color=='r'){
                w->color='b';
                x->p->color='r';
                Right_rotate(T, x->p);
                w=x->p->left;
            }
            if(w->right->color=='b' && w->left->color=='b'){
                w->color='r';
                Tree_delete_fixup(T, x->p);
            }else {
                if(w->left->color=='b'){
                    w->right->color='b';
                    w->color='r';
                    Left_rotate(T, w);
                    w=x->p->left;
                }
                w->color=x->p->color;
                x->p->color='b';
                Right_rotate(T, x->p);
            }
        }
    }
}


int stringcompare(char* str1, char* str2){
    int i, flag=0;
    char c1, c2;
    c1=str1[0];
    i=0;
    while(c1!='\0'){
        c1=str1[i];
        c2=str2[i];
        if(c1!=c2){
            if(c1<c2){
                flag=-1;
            }
            else{
                flag=1;
            }
            break;
        }
         i++;
    }
    return flag;
}

