#ifndef _read_results_file_INCLUDED
#define _read_results_file_INCLUDED

#include<string>
#include<vector>

//Node object used to store data for both the input FE mesh and output regular grid points
class node
{
  public:
    int id, coord_array_created; 
    double x, y, z, temperature; 
    double cd_vec[3];
	
    node(double, double, double, int, int);
    void printinfo(void); 
    void add_temperature(double);
    void create_coordinate_array(void);
};
    
//Element object used to store the FE unstructured mesh data 	
class element
{
  public:
    int v1,v2,v3,v4,id; //4 vertex node IDs and element ID
    double minx, miny, minz, maxx, maxy, maxz;
    element(int,int,int,int,int,int,int);
    void printinfo(void);
    void find_bounds(void);
    bool is_point_inside(node *point);
    double interpret_pt_value(node *point);
}; 
   
//Cube object used for creating the vtu output visualization of the regular grid temp transfer
class cube
{
  public:
    int id_list[8];
	cube(int*);
};


//Math functions used to determine if the regular gridpoints are inside the FE tet elements
double dot_prod(double v_a[], double v_b[]);
void cross_prod(double cross_p[], double v_a[], double v_b[]);
void vector_subtract(double subtr[], double v_a[], double v_b[]);
bool SameSide(double v1c[], double v2c[], double v3c[], double v4c[], double p[]);
bool PointInTetrahedron(double v1c[], double v2c[], double v3c[], double v4c[], double p[]);


//Extracts the FE unstructured mesh data from the Albany vtu output files
bool get_FEM_vtu_data(std::string results_folder, int num_parts, int time_iter, std::string data_name_list, bool single_val, int pt_val);

//Global vars to hold FE unstructured mesh data - may want to change in the future and make local
extern std::vector<node> nodelist;
extern std::vector<element> elementlist;

#endif
