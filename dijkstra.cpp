#include <iostream>
#include <vector>
#include <string>
#include <algorithm>    // std::sort
#include <stdio.h>
#include <time.h>
#include <ctime>
#include <stdlib.h>
#include <iomanip>
#include <cmath>
#define PI 3.14159265358979323846
#include <chrono>
#include <random>


unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine e(seed);


// for the usleep function
#include <unistd.h>
using namespace std;

bool grid_mode = false;
bool name_mode = true;

class Node;
class Triplet;


vector<Node*> nodes_vector;

class Node
{
public:
  Node();
  Node(Node*& n); // copy constructor
  //Node( int x0, int y0 );
  Node( int x0, int y0, string * s );
  ~Node();
  friend ostream& operator <<(ostream& out, const Node& n);
  void setName(string * s);
  string * getName(){ return name; };
  void addLink( Node* n );
  void addBackLink( Node* n );
  void setX( int i ){ x = i; };
  void setY( int i ){ y = i; };
  void setXY( int i, int j ){ x = i; y = j; };
  void setCharacter( string * s ){ delete character; character = s; };
  string * getCharacter(){ return character; };
  int getTroops(){ return troops; };
  void setTroops( int i ){ troops = i; };
  vector<Node*> getLinks(){ return links; };
  void setChildren( vector<Node*> v ){ children = v; };
  vector<Node*> getChildren(){ return children; };
  friend bool operator==(const Node& lhs, const Node& rhs);
private:
  
  string * name;
  string * character;
  vector<Node*> links;
  vector<Node*> parents;
  vector<Node*> children;
  int x;
  int y;
  int troops;
};


bool operator==(const Node& lhs, const Node& rhs)
{
  bool return_value = false;
  if( lhs.x == rhs.x && lhs.y == rhs.y && lhs.name == rhs.name && lhs.character == rhs.character && lhs.links == rhs.links ) return_value = true;
  return return_value;
}

void Node::addLink( Node * n )
{
  bool found=false;
  if( n == this )
    {
#ifdef DEBUG
      cerr << "cannot link to self" << endl;
#endif
      return;
    }
  for( vector<Node*>::iterator i=links.begin(); i<links.end(); i++ )
    {
      if( n == links[distance( links.begin(), i )] )
	{
	  found=true;
	  i=links.end();
	}      
    }
  if( !found )
    {
      links.push_back( n );
      n->addBackLink( this );
    }
#ifdef DEBUG
  else
    {
      cerr << "cannot link - one already exists" << endl;
    }
#endif
  return;
}


void Node::addBackLink( Node * n )
{
#ifdef DEBUG
  cerr << "adding back-link" << endl;
#endif
  links.push_back( n );
  return;
}

ostream& operator <<(ostream& out, const Node& n)
{
  if( grid_mode )
    { 
      out << "\e[" << 13-n.y << ";" << 13+n.x << "H" << *n.character;
    }
  else if( name_mode )
    {
      out << *n.name;
    }
  else
    {
      out << "(" << n.x << "," << n.y << ")";
    }
  return out;
};

void Node::setName(string * s)
{
  delete name;
  name = s;
}

Node::Node(Node*& n)
{
  
  x = n->x;
  y = n->y;
  name = new string( *n->name );
  character = new string( *n->character );
  troops = n->troops;
  for( vector<Node*>::iterator i=n->children.begin(); i<n->children.end(); i++ )
    {
      children.push_back( new Node(*n->children[distance( n->children.begin(), i )]) );
    }
  
}

Node::Node( int x0, int y0, string * s = new string("unnamed") )
{
  x=x0;
  y=y0;
  name = s;
  character = new string( "|" );
  troops = rand()%6+1;
}

Node::Node()
{
  name = new string( "unnamed" );
}

Node::~Node()
{
#ifdef DEBUG
  cerr <<  "deleting node and all of its children and parents: " << *this << endl;
#endif
  x=0;
  y=0;
  troops=0;
  delete name;
  delete character;
  for( vector<Node*>::iterator i=children.begin(); i<children.end(); i++ )
    {
      delete children[distance(children.begin(), i )];
    }
}

// HELPER FUNCTIONS
void printAllNodes()
{
  for( vector<Node*>::iterator i=nodes_vector.begin(); i<nodes_vector.end(); i++ )
    {
      cout << *nodes_vector[distance( nodes_vector.begin(), i )] << endl;
    }
}

void cls()
{
  // ansi cls
  cout << "\e[2J";
}

void deleteAllNodes()
{
  for( vector<Node*>::iterator i=nodes_vector.begin(); i<nodes_vector.end(); i++ )
    {
      delete nodes_vector[distance( nodes_vector.begin(), i )];
    }
  
  /* delete them all */
  nodes_vector.erase(nodes_vector.begin(), nodes_vector.end());
}



// ostream& operator <<(ostream& out, const Path& p)
// {  
//   for( int i=0;i<p.getSize();i++ )
//     {
//       out << p.v[i] << " ";
//     }
//   return out;
// }
  

/*
  To know how Dijkstra's algorithm works behind the scene, look at the below steps to understand it in detail:

  First of all, we will mark all vertex as unvisited vertex
  Then, we will mark the source vertex as 0 and all other vertices as infinity
  Consider source vertex as current vertex
  Calculate the path length of all the neighboring vertex from the current vertex by adding the weight of the edge in the current vertex
  Now, if the new path length is smaller than the previous path length then replace it otherwise ignore it
  Mark the current vertex as visited after visiting the neighbor vertex of the current vertex
  Select the vertex with the smallest path length as the new current vertex and go back to step 4.
  Repeat this process until all the vertex are marked as visited.
  Once we go through the algorithm, we can backtrack the source vertex and find our shortest path.
*/
class Triplet
{
public:
  Triplet(){};
  Triplet( Node * n, int i );
  ~Triplet(){};
  Node * getNode(){ return node; };
  int getWeight(){ return weight; };
  void setWeight(int i){weight=i;};
  friend ostream& operator <<(ostream& out, const Triplet& t);

private:
  Node * node;
  int weight;
  int id;  
};


ostream& operator <<(ostream& out, const Triplet& t)
{
  out << *t.node << " wt: " << t.weight; 
  return out;
}

Triplet::Triplet( Node * n, int i )
{
  node = n;
  weight = i;
}

int depth=0;
vector<Triplet*> visited_vector;
vector<Node*> path;
vector<Node*> lineage;


int find( Node* n )
{
  int return_value = -1;
  for( vector<Triplet*>::iterator j=visited_vector.begin(); j<visited_vector.end(); j++ )
    {
      Triplet * t = visited_vector[distance(visited_vector.begin(), j)];
      if( t->getNode() == n )
	{
	  return_value = distance(visited_vector.begin(), j);
	}
    }
  return return_value;
}

bool alreadyVisited( Node* n )
{
  for( vector<Triplet*>::iterator j=visited_vector.begin(); j<visited_vector.end(); j++ )
    {
      Triplet * t = visited_vector[distance(visited_vector.begin(), j)];
      if( t->getNode() == n )
	{
	  return true;
	}
    }
  return false;
}

bool isInLineage( Node* n )
{
  for( int i=0; i<lineage.size(); i++ )
    {
      if( lineage[i] == n ) return true;
    }
  return false;
}

void dump(vector<Node*> v)
{
  for( int i=0;i<v.size();i++)
    cout << *v[i] << " ";
  cout << endl;
}

void spaces(int i)
{
  for( int j=0; j<i; j++ ) cout << "\t";
}

Triplet * getFromVisitedVector( Node * n )
{
  for( int i = 0; i < visited_vector.size(); i++ )
    {
      if( visited_vector[i]->getNode() == n )
	{
	  return visited_vector[i];
	}
    }
  return NULL;
}

int goal_count = 0;
void getEasiestPath( Node* start, Node* end, int weight_total=0)
{
  // before making this a "visited node" check to see if
  // this node is already in the visited_vector
  // 
  // compare it's weight with the current weight_total
  //
  // if the weight that is in the vector is greater than
  // this weight_total, then change the weight that is
  // in the vector
  //
  // OTHERWISE (if the weight in the vector is <= weight_total)
  //
  // return from function
  
  if( alreadyVisited( start ) )
    {
#ifdef DEBUG
      cout << *start << " already visited" << endl;
#endif
      Triplet * t = getFromVisitedVector( start );
      int w = t->getWeight();
      if( w > weight_total )
	{
	  t->setWeight(weight_total);
	}
      else
	{
	  // don't bother continuing down this line
	  // because we're already further from the
	  // start than a previously discovered node
	  return;
	}
    }
  else
    {
      Triplet * t = new Triplet( start, weight_total );
#ifdef DEBUG
      cout << "created new triplet: " << *t << endl;
#endif
      visited_vector.push_back( t );
    }


  lineage.push_back( start );
#ifdef DEBUG
  dump( lineage );
#endif

  vector<Node*> l = start->getLinks();
  weight_total += start->getTroops();
  
  if( start == end )
    {
#ifdef DEBUG
      spaces(depth); cout << "*** GOAL reached: " << *start << "\t\tweight: " << weight_total << " ***" << endl;
#endif
      if( path.size() > 0 )
	{
	  // erase path
	  path.erase(path.begin(), path.end() );
	}
      
      for( int i=0; i<lineage.size(); i++ )
	{
	  path.push_back( lineage[i] );
	}
      
      goal_count++;
    }
#ifdef DEBUG
  else if( l.size() == 1 )
    {
      spaces(depth); cout << "*** DEAD end ***" << endl;
    }
#endif
  else
    {
      for( int j = 0; j<l.size(); j++ )
	{
	  if( !isInLineage(l[j]) )
	    {
	      getEasiestPath( l[j], end, weight_total );
	    }
	  
	  
	}
    }

  lineage.pop_back();	      
  return;
}

void link( int a, int b )
{
  nodes_vector[a]->addLink( nodes_vector[b] );
}

void setWeight( int i, int j )
{
#ifdef DEBUG
  cerr << "setting the weight of " << *nodes_vector[i] << " to " << j << endl;
#endif
  nodes_vector[i]->setTroops( j );
}

Node * getBacktrackNode( Node * n )
{
  vector<Node*> l = n->getLinks();
  int which_is_closer_to_home;
  
  int min=getFromVisitedVector(l[0])->getWeight()+1;

  for( int i=0; i < l.size(); i++ )
    {
      if( getFromVisitedVector(l[i])->getWeight() < min )
	{
	  which_is_closer_to_home = i;
	}
    }
  return getFromVisitedVector(l[which_is_closer_to_home])->getNode();  
}

int main()
{
  //cls();
  srand( time(NULL) );

#ifdef DEBUG
  cerr << "populating" << endl;
#endif
  nodes_vector.push_back( new Node( 5, 5, new string( "A" ) ) );   // 0
  nodes_vector.push_back( new Node( 10, 4, new string( "B" ) ) );  // 1
  nodes_vector.push_back( new Node( 15, 5, new string( "C" ) ) );  // 2
  nodes_vector.push_back( new Node( 20, 4, new string( "D" ) ) );  // 3
  nodes_vector.push_back( new Node( 5, 2, new string( "G" ) ) );   // 4
  nodes_vector.push_back( new Node( 10, 1, new string( "H" ) ) );  // 5
  nodes_vector.push_back( new Node( 15, 3, new string( "E" ) ) );  // 6
  nodes_vector.push_back( new Node( 20, 0, new string( "J" ) ) );  // 7
  nodes_vector.push_back( new Node( 15, 0, new string( "I" ) ) );  // 8
  nodes_vector.push_back( new Node( 0, 0, new string( "F" ) ) );   // 9 
  nodes_vector.push_back( new Node( 18, -2, new string( "K" ) ) ); // 10
  nodes_vector.push_back( new Node( 20, -2, new string( "L" ) ) ); // 11
  nodes_vector.push_back( new Node( 20, -5, new string( "M" ) ) ); // 12
  
  nodes_vector.push_back( new Node( 22, -4, new string( "N" ) ) ); // 13
  nodes_vector.push_back( new Node( 22, -7, new string( "O" ) ) ); // 14

  

#ifdef DEBUG
  cerr << "linking" << endl;
#endif
  link( 0, 1 );
  link( 0, 4 );
  
  link( 1, 6 );
  link( 1, 4 );

  link( 2, 5 );
  link( 2, 6 );

  link( 3, 7 );
  link( 3, 6 );

  link( 4, 5 );
  link( 4, 9 );

  link( 5, 6 );
  link( 5, 8 );

  link( 6, 7 );
  link( 6, 10 );

  link( 7, 8 );
  link( 7, 11 );

  link( 8, 11 );

  link( 10, 11 );
  link( 10, 12 );

  link( 11, 12 );
  link( 10, 13 );
  link( 11, 13 );
  link( 12, 13 );
  link( 14, 13 );

  
#ifdef DEBUG
  cerr << "done linking nodes" << endl;
#endif
  
  setWeight( 0, 0 );
  setWeight( 1, 2 );
  setWeight( 2, 3 );
  setWeight( 3, 1 );
  setWeight( 4, 1 );
  setWeight( 5, 2 );
  setWeight( 6, 2 );
  setWeight( 7, 3 );
  setWeight( 8, 3 );
  setWeight( 9, 3 );
  setWeight( 10, 2 );
  setWeight( 11, 1 );
  setWeight( 12, 5 );
  setWeight( 13, 1 );
  setWeight( 14, 1 );
#ifdef DEBUG
  cerr << "done setting weights" << endl;
#endif
  
  getEasiestPath( nodes_vector[0], nodes_vector[12], 0 );
#ifdef DEBUG  
  cout << "found " << goal_count << " solutions" << endl;
#endif

  for( int i=0; i<path.size(); i++ )
     {
       cout << *path[i];
       if( i < path.size()-1) cout << "->";
     }
   cout << endl;
  return 0;
}
