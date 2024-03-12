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


// NORTH AMERICA
#define ALASKA 0
#define NORTHWEST_TERRITORY 1
#define ALBERTA 2
#define GREENLAND 3
#define ONTARIO 4
#define QUEBEC 5
#define WESTERN_US 6
#define EASTERN_US 7
#define CENTRAL_AMERICA 8
#define ICELAND 9

// SOUTH AMERICA
#define VENEZUELA 10
#define PERU 11
#define ARGENTINA 12
#define BRAZIL 13

// EUROPE
#define GREAT_BRITAIN 14
#define SCANDANAVIA 15
#define NORTHERN_EUROPE 16
#define WESTERN_EUROPE 17
#define SOUTHERN_EUROPE 18
#define UKRAINE 19

// AFRICA
#define NORTH_AFRICA 20
#define EAST_AFRICA 21
#define EGYPT 22
#define CONGO 23
#define SOUTH_AFRICA 24
#define MADAGASCAR 25

// AUSTRALIA
#define NEW_GUINEA 26
#define INDONESIA 27
#define WESTERN_AUSTRALIA 28
#define EASTERN_AUSTRALIA 29

// ASIA
#define URAL 30
#define SIBERIA 31
#define YAKUTSK 32
#define IRKUTSK 33
#define JAPAN 34
#define MONGOLIA 35
#define AFGHANISTAN 36
#define THE_MIDDLE_EAST 37
#define INDIA 38
#define CHINA 39
#define SIAM 40
#define KAMCHATKA 41


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
    cout << "[" << *v[i] << "] ";
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


int main()
{
  //cls();
  srand( time(NULL) );

#ifdef DEBUG
  cerr << "populating" << endl;
#endif
  nodes_vector.push_back( new Node( 1, 0, new string( "Alaska" ) ) );   // 0
  nodes_vector.push_back( new Node( 1, 1, new string( "Northwest Territory" ) ) );  // 1
  nodes_vector.push_back( new Node( 2, 0, new string( "Alberta" ) ) );  // 2
  nodes_vector.push_back( new Node( 1, 2, new string( "Greenland" ) ) );  // 3
  nodes_vector.push_back( new Node( 2, 1, new string( "Ontario" ) ) );   // 4
  nodes_vector.push_back( new Node( 2, 2, new string( "Quebec" ) ) );  // 5
  nodes_vector.push_back( new Node( 3, 0, new string( "Western U.S." ) ) );  // 6
  nodes_vector.push_back( new Node( 3, 1, new string( "Eastern U.S." ) ) );  // 7
  nodes_vector.push_back( new Node( 4, 1, new string( "Central America" ) ) );  // 8
  nodes_vector.push_back( new Node( 1, 3, new string( "Iceland" ) ) );   // 9
  
  nodes_vector.push_back( new Node( 4, 2, new string( "Venezuela" ) ) ); // 10
  nodes_vector.push_back( new Node( 5, 2, new string( "Peru" ) ) ); // 11
  nodes_vector.push_back( new Node( 5, 3, new string( "Argentina" ) ) ); // 12
  nodes_vector.push_back( new Node( 4, 3, new string( "Brazil" ) ) ); // 13

  
  nodes_vector.push_back( new Node( 2, 3, new string( "Great Britain" ) ) ); 
  nodes_vector.push_back( new Node( 1, 4, new string( "Scandanavia" ) ) ); 
  nodes_vector.push_back( new Node( 2, 4, new string( "N. Europe" ) ) ); 
  nodes_vector.push_back( new Node( 3, 4, new string( "W. Europe" ) ) ); 
  nodes_vector.push_back( new Node( 2, 5, new string( "S. Europe" ) ) ); 
  nodes_vector.push_back( new Node( 1, 5, new string( "Ukraine" ) ) ); 

  nodes_vector.push_back( new Node( 3, 5, new string( "N. Africa" ) ) ); 
  nodes_vector.push_back( new Node( 4, 6, new string( "E. Africa" ) ) ); 
    nodes_vector.push_back( new Node( 3, 6, new string( "Egypt" ) ) ); 
  nodes_vector.push_back( new Node( 4, 5, new string( "Congo" ) ) ); 
  nodes_vector.push_back( new Node( 5, 5, new string( "S. Africa" ) ) ); 
  nodes_vector.push_back( new Node( 5, 6, new string( "Madagascar" ) ) ); 

    nodes_vector.push_back( new Node( 4, 9, new string( "New Guinea" ) ) ); 
  nodes_vector.push_back( new Node( 4, 8, new string( "Indonesia" ) ) ); 
  nodes_vector.push_back( new Node( 5, 8, new string( "W Australia" ) ) ); 
  nodes_vector.push_back( new Node( 5, 9, new string( "E Australia" ) ) ); 

  nodes_vector.push_back( new Node( 0, 6, new string( "Ural" ) ) ); 
  nodes_vector.push_back( new Node( 0, 7, new string( "Siberia" ) ) ); 
  nodes_vector.push_back( new Node( 0, 8, new string( "Yakutsk" ) ) ); 
  nodes_vector.push_back( new Node( 1, 8, new string( "Irkutsk" ) ) ); 
  nodes_vector.push_back( new Node( 1, 9, new string( "Japan" ) ) ); 
  nodes_vector.push_back( new Node( 2, 8, new string( "Mongolia" ) ) ); 
  nodes_vector.push_back( new Node( 1, 6, new string( "Afghan" ) ) ); 
  nodes_vector.push_back( new Node( 2, 6, new string( "The Middle East" ) ) ); 
  nodes_vector.push_back( new Node( 2, 7, new string( "India" ) ) ); 
  nodes_vector.push_back( new Node( 1, 7, new string( "China" ) ) ); 
  nodes_vector.push_back( new Node( 3, 7, new string( "Siam" ) ) ); 
    nodes_vector.push_back( new Node( 0, 9, new string( "Kamchatka" ) ) ); 


#ifdef DEBUG
  cerr << "linking" << endl;
#endif
  link( ALASKA, NORTHWEST_TERRITORY );
  link( ALASKA, ALBERTA );
  link( ALASKA, KAMCHATKA );

  



  link( NORTHWEST_TERRITORY, ALASKA );
  link( NORTHWEST_TERRITORY, ALBERTA );
  link( NORTHWEST_TERRITORY, ONTARIO );
  link( NORTHWEST_TERRITORY, GREENLAND );

  link( ALBERTA, ALASKA );
  link( ALBERTA, NORTHWEST_TERRITORY );
  link( ALBERTA, ONTARIO );
  link( ALBERTA, WESTERN_US );
  
  link( GREENLAND, NORTHWEST_TERRITORY );
  link( GREENLAND, ONTARIO );
  link( GREENLAND, QUEBEC );
  link( GREENLAND, ICELAND );

  link( ONTARIO, NORTHWEST_TERRITORY );
  link( ONTARIO, GREENLAND );
  link( ONTARIO, QUEBEC );
  link( ONTARIO, EASTERN_US );
  link( ONTARIO, WESTERN_US );
  link( ONTARIO, ALBERTA );

  link( QUEBEC, EASTERN_US );
  link( QUEBEC, ONTARIO );
  link( QUEBEC, GREENLAND );

  link( WESTERN_US, ALBERTA );
  link( WESTERN_US, ONTARIO );
  link( WESTERN_US, EASTERN_US );
  link( WESTERN_US, CENTRAL_AMERICA );
  
  link( EASTERN_US, WESTERN_US );
  link( EASTERN_US, CENTRAL_AMERICA );
  link( EASTERN_US, ONTARIO );
  link( EASTERN_US, QUEBEC );

  link( CENTRAL_AMERICA, WESTERN_US );
  link( CENTRAL_AMERICA, EASTERN_US );
  link( CENTRAL_AMERICA, VENEZUELA );

  link( VENEZUELA, CENTRAL_AMERICA );
  link( VENEZUELA, BRAZIL );
  link( VENEZUELA, PERU );

  link( BRAZIL, VENEZUELA );
  link( BRAZIL, PERU );
  link( BRAZIL, ARGENTINA );
  link( BRAZIL, NORTH_AFRICA );

  link( PERU, VENEZUELA );
  link( PERU, BRAZIL );
  link( PERU, ARGENTINA );

  link( ARGENTINA, PERU );
  link( ARGENTINA, BRAZIL );
  
  link( ICELAND, GREAT_BRITAIN );
  link( ICELAND, GREENLAND );
  link( ICELAND, SCANDANAVIA );

  link( KAMCHATKA, ALASKA );
  link( KAMCHATKA, JAPAN );
  link( KAMCHATKA, MONGOLIA );
  link( KAMCHATKA, IRKUTSK );
  link( KAMCHATKA, YAKUTSK );

  link( JAPAN, MONGOLIA );
  link( JAPAN, KAMCHATKA );


  link( AFGHANISTAN, UKRAINE );
  link( AFGHANISTAN, URAL );
  link( AFGHANISTAN, CHINA );
  link( AFGHANISTAN, INDIA );
  link( AFGHANISTAN, THE_MIDDLE_EAST );
 
  
  link( CHINA, SIAM );
  link( CHINA, INDIA );
  link( CHINA, AFGHANISTAN );
  link( CHINA, URAL );
  link( CHINA, SIBERIA );
  link( CHINA, MONGOLIA );

  link( MONGOLIA, JAPAN );
  link( MONGOLIA, CHINA );
  link( MONGOLIA, SIBERIA );
  link( MONGOLIA, IRKUTSK );
  link( MONGOLIA, KAMCHATKA );

  link( IRKUTSK, MONGOLIA );
  link( IRKUTSK, KAMCHATKA );
  link( IRKUTSK, YAKUTSK );
  link( IRKUTSK, SIBERIA );

  link( YAKUTSK, KAMCHATKA );
  link( YAKUTSK, SIBERIA );
  link( YAKUTSK, IRKUTSK );

  link( SIBERIA, URAL );
  link( SIBERIA, CHINA );
  link( SIBERIA, MONGOLIA );
  link( SIBERIA, IRKUTSK );
  link( SIBERIA, YAKUTSK );

  link( INDIA, THE_MIDDLE_EAST );
  link( INDIA, AFGHANISTAN );
  link( INDIA, CHINA );
  link( INDIA, SIAM );

  link( THE_MIDDLE_EAST, EGYPT );
  link( THE_MIDDLE_EAST, SOUTHERN_EUROPE );
  link( THE_MIDDLE_EAST, UKRAINE );
  link( THE_MIDDLE_EAST, AFGHANISTAN );
  link( THE_MIDDLE_EAST, INDIA );
  link( THE_MIDDLE_EAST, EAST_AFRICA );

  link( URAL, UKRAINE );
  link( URAL, AFGHANISTAN );
  link( URAL, CHINA );
  link( URAL, SIBERIA );

  link( SIAM, INDIA );
  link( SIAM, CHINA );
  link( SIAM, INDONESIA );


  link( EGYPT, THE_MIDDLE_EAST );
  link( EGYPT, SOUTHERN_EUROPE );
  link( EGYPT, NORTH_AFRICA );
  link( EGYPT, EAST_AFRICA );

  link( EAST_AFRICA, EGYPT );
  link( EAST_AFRICA, THE_MIDDLE_EAST );
  link( EAST_AFRICA, MADAGASCAR );
  link( EAST_AFRICA, SOUTH_AFRICA );
  link( EAST_AFRICA, CONGO );
  link( EAST_AFRICA, NORTH_AFRICA );

  link( MADAGASCAR, EAST_AFRICA );
  link( MADAGASCAR, SOUTH_AFRICA );

  link( SOUTH_AFRICA, MADAGASCAR );
  link( SOUTH_AFRICA, EAST_AFRICA );
  link( SOUTH_AFRICA, CONGO );

  link( CONGO, NORTH_AFRICA );
  link( CONGO, EAST_AFRICA );
  link( CONGO, SOUTH_AFRICA );

  link( NORTH_AFRICA, BRAZIL );
  link( NORTH_AFRICA, WESTERN_EUROPE );
  link( NORTH_AFRICA, SOUTHERN_EUROPE );
  link( NORTH_AFRICA, EGYPT );
  link( NORTH_AFRICA, EAST_AFRICA );
  link( NORTH_AFRICA, CONGO );

  link( INDONESIA, SIAM );
  link( INDONESIA, NEW_GUINEA );
  link( INDONESIA, WESTERN_AUSTRALIA );

  link( NEW_GUINEA, INDONESIA );
  link( NEW_GUINEA, WESTERN_AUSTRALIA );
  link( NEW_GUINEA, EASTERN_AUSTRALIA );

  link( WESTERN_AUSTRALIA, INDONESIA );
  link( WESTERN_AUSTRALIA, NEW_GUINEA );
  link( WESTERN_AUSTRALIA, EASTERN_AUSTRALIA );

  link( EASTERN_AUSTRALIA, NEW_GUINEA );
  link( EASTERN_AUSTRALIA, WESTERN_AUSTRALIA );

  link( GREAT_BRITAIN, ICELAND );
  link( GREAT_BRITAIN, SCANDANAVIA );
  link( GREAT_BRITAIN, NORTHERN_EUROPE );
  link( GREAT_BRITAIN, WESTERN_EUROPE );
  
  link( SCANDANAVIA, ICELAND );
  link( SCANDANAVIA, GREAT_BRITAIN );
  link( SCANDANAVIA, UKRAINE );
  link( SCANDANAVIA, NORTHERN_EUROPE );

  link( UKRAINE, SCANDANAVIA );
  link( UKRAINE, NORTHERN_EUROPE );
  link( UKRAINE, SOUTHERN_EUROPE );
  link( UKRAINE, THE_MIDDLE_EAST );
  link( UKRAINE, AFGHANISTAN );
  link( UKRAINE, URAL );

  link( NORTHERN_EUROPE, SCANDANAVIA );
  link( NORTHERN_EUROPE, GREAT_BRITAIN );
  link( NORTHERN_EUROPE, WESTERN_EUROPE );
  link( NORTHERN_EUROPE, SOUTHERN_EUROPE );
  link( NORTHERN_EUROPE, UKRAINE );


  link( SOUTHERN_EUROPE, THE_MIDDLE_EAST );
  link( SOUTHERN_EUROPE, UKRAINE );
  link( SOUTHERN_EUROPE, NORTHERN_EUROPE );
  link( SOUTHERN_EUROPE, WESTERN_EUROPE );
  link( SOUTHERN_EUROPE, NORTH_AFRICA );
  link( SOUTHERN_EUROPE, EGYPT );

  link( WESTERN_EUROPE, GREAT_BRITAIN );
  link( WESTERN_EUROPE, NORTHERN_EUROPE );
  link( WESTERN_EUROPE, SOUTHERN_EUROPE );
  link( WESTERN_EUROPE, NORTH_AFRICA );



  
#ifdef DEBUG
  cerr << "done linking nodes" << endl;
#endif

  for( int i=0; i<nodes_vector.size(); i++ )
    {
      setWeight( i, rand()%6+2 );
      //setWeight( i, 1 );
    }
  
#ifdef DEBUG
  cerr << "done setting weights" << endl;
#endif
  
  getEasiestPath( nodes_vector[ALASKA], nodes_vector[MADAGASCAR], 0 );
#ifdef DEBUG  
  cout << "found " << goal_count << " solutions" << endl;
#endif

  for( int i=0; i<path.size(); i++ )
     {
       cout << "[" << *path[i] << "]";
       if( i < path.size()-1) cout << " -> ";
     }
   cout << endl;
  return 0;
}
