#include "exp_tree.h"
#include <set>
#include <stdio.h>
#include <string.h>


long n_id=0;
std::string symbols;
std::set <tree_point*> t_leaves;

tree_point::tree_point()
  : left(0)
  , right(0)
  , parent(0)
{}
tree_point::tree_point(char c)
  : left(0)
  , right(0)
  , parent(0)
  , contents(c)
  , type(symbol)
  , nullable(false)
{
}
tree_point::tree_point(unsigned int c1,unsigned int c2)
  {
    type=symbol;
    tree_point* prev = new tree_point(c1);
    for (unsigned int i=c1+1;i<=c2;i++)
    {
      tree_point* l = prev;
      tree_point* r = new tree_point(i);
      prev = new tree_point();
      prev->left=l;
      prev->right=r;
      prev->type=opUnion;
      l->parent=r->parent=prev;      
    }
    contents=prev->contents;
    left=prev->left;
    right=prev->right;
    parent=prev->parent;
    nullable=false;
    type=prev->type;
  }
tree_point::tree_point(tree_point*tp)
  {
    left=tp->left;
    right=tp->right;
    parent=tp->parent;
    type=tp->type;
    contents=tp->contents;
    nullable=tp->nullable;
    firstpos=tp->firstpos;
  }
tree_point::~tree_point()
{

  delete left;
  if (0)
  {
    if (right && left)
    {
      if (this->id == 25)
        id = id;
    }

    if (right)
      delete right;
    if (this->type !=opIter && left)
      delete left;
    firstpos.clear();
    followpos.clear();
    lastpos.clear();
    memset(this,NULL,sizeof(this));

  }
}

void tree_point::calc()
{
  id=n_id++;
  if (right && left)
  {
    right->calc();
    if (type!=opIter)
      left->calc();
    switch(type)
    {
      case opUnion:
        {
          firstpos.insert(left->firstpos.begin(),left->firstpos.end());
          firstpos.insert(right->firstpos.begin(),right->firstpos.end());
          lastpos.insert(left->lastpos.begin(),left->lastpos.end());
          lastpos.insert(right->lastpos.begin(),right->lastpos.end());
          break;
        }
      case opIter:
        {
          firstpos=left->firstpos;
          lastpos=left->lastpos;
          for (std::set <tree_point*>::iterator it = lastpos.begin(); it != lastpos.end();  ++it)
              (*it)->followpos.insert(firstpos.begin(),firstpos.end());
          break;
        }
      case opConcat:
        {
          if (left->nullable)
          {
            firstpos.insert(left->firstpos.begin(),left->firstpos.end());
            firstpos.insert(right->firstpos.begin(),right->firstpos.end());
          }
          else
            firstpos=left->firstpos;
          if (right->nullable)
          {
            lastpos.insert(left->lastpos.begin(),left->lastpos.end());
            lastpos.insert(right->lastpos.begin(),right->lastpos.end());
          }
          else
            lastpos=right->lastpos;
          for (std::set <tree_point*>::iterator it = left->lastpos.begin(); it != left->lastpos.end();  ++it)
            (*it)->followpos.insert(right->firstpos.begin(),right->firstpos.end());
          break;
        }
    }
  }
  else if (type==symbol)
  {
    firstpos.insert(this);
    lastpos.insert(this);
    t_leaves.insert(this);
    if(!(contents=='#' && end == 1) && symbols.find(contents) == std::string::npos)
      symbols.push_back(contents);
  }
    
}
void tree_point::print(int n)
{
  for(int i=0;i<n;i++)
    printf("%s","   ");
  switch(type)
  {
    case symbol:
      printf("%c",contents);
      break;
    case opUnion:
      printf("%s","|");
      break;
    case opIter:
      printf("%s","*");
      break;
    case opConcat:
      printf("%s","&");
      break;
    case empty:
      printf("%s","<e>");
      break;
  }

  if(right)
    right->print(n+1);
  if(left)
    left->print(n+1);
}
//****************************************************************
exp_tree::exp_tree()
  {
    root=NULL;
  }

exp_tree::exp_tree(exp_tree* t)
{
  root = t->root;
  this->alphabet = t->alphabet;
  this->leaves = t->leaves;
}

exp_tree::exp_tree(tree_point pnt)
  {
    root=new tree_point();
    root->contents=pnt.contents;
    root->id=pnt.id;
    root->left=pnt.left;
    root->parent=pnt.parent;
    root->right=pnt.right;
    root->type=pnt.type;
    root->nullable=pnt.nullable;
    root->firstpos=pnt.firstpos;
    root->followpos=pnt.followpos;
    root->lastpos=pnt.lastpos;

  }
exp_tree::exp_tree(unsigned int c1,unsigned int c2)
  {
    root=new tree_point(c1,c2);
  }
exp_tree::exp_tree(char c)
  {
    root=new tree_point(c);
  }
exp_tree::exp_tree(point_type t)
  {
    root = new tree_point();
    root->type=t;
    root->nullable = (t==empty || t==opIter) ? true : false;

  }
bool exp_tree::is_empty()
  {
    return root==NULL;
  }
exp_tree* exp_tree::make_new_root(point_type t)
  {
    tree_point* pnt = new tree_point();
    pnt->type=t;
    pnt->right=pnt->left=root;
    root->parent=pnt;
    pnt->nullable = t==opIter ? true : pnt->right->nullable;
    root=new tree_point(pnt);
    exp_tree* out = new exp_tree(); 
    out->root=root;
    return out;
  }
exp_tree* exp_tree::merge_trees(exp_tree* l,exp_tree* r,point_type t)
  {
    exp_tree* res = new exp_tree(t);
    l->root->parent=r->root->parent=res->root;
    res->root->left=l->root;
    res->root->right=r->root;
    res->root->nullable = t==opIter ? true : (t==opConcat ? (res->root->right->nullable && res->root->left->nullable) : (res->root->right->nullable || res->root->left->nullable));
    return res;
  }
void exp_tree::calc_followpos()
{
  symbols="";
  t_leaves.clear();
  n_id=0;
  root->id=0;
  root->calc();
  alphabet=symbols;
  leaves=t_leaves;
}

exp_tree::~exp_tree()
{
  delete root;
}
