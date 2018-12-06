#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<vector>
#include<cmath>
#include<iomanip>
#include<sstream>
#include<algorithm>
#include "read_results_file.hpp"

using namespace std;

std::vector<node> nodelist;
std::vector<element> elementlist;

cube::cube(int *array_ptr){
	for (int i = 0; i < 8; i++){
		id_list[i] = array_ptr[i];
	}
}

node::node(double a, double b, double c, int k, int offset){
  x = a;
  y = b;
  z = c;
  id = k + offset;
  coord_array_created = 0;
}

void node::printinfo(void){
  std::cout << fixed << setprecision(10) << "Node ID: " << id << "  X coordinate: " << x << "  Y Coordinate: " << y << " Z Coordinate: " << z << "  Temp: " << temperature << endl;
}

void node::add_temperature(double value){
  temperature = value;
}

void node::create_coordinate_array(void){
  cd_vec[0] = x;
  cd_vec[1] = y;
  cd_vec[2] = z;
  coord_array_created = 1;
}          

element::element(int a, int b, int c, int d, int k, int offset_el, int offset_pt){
  v1 = a + offset_pt; 
  v2 = b + offset_pt; 
  v3 = c + offset_pt; 
  v4 = d + offset_pt;
  id = k + offset_el;
}

void element::printinfo(void){
  std::cout << fixed << setprecision(10) << "Elem ID: " << id << "  Vert 1 ID: " << v1 << "  Vert 2 ID: " << v2 << " Vert 3 ID: " << v3 << " Vert4 ID: " << v4 << endl;
  //std::cout << "Vert1 coords" << 
}

void element::find_bounds(void){
  double temp_array_x[] = {nodelist[v1].x, nodelist[v2].x, nodelist[v3].x, nodelist[v4].x};  
  double temp_array_y[] = {nodelist[v1].y, nodelist[v2].y, nodelist[v3].y, nodelist[v4].y};
  double temp_array_z[] = {nodelist[v1].z, nodelist[v2].z, nodelist[v3].z, nodelist[v4].z};
  maxx = *std::max_element(temp_array_x, temp_array_x+4);
  maxy = *std::max_element(temp_array_y, temp_array_y+4);
  maxz = *std::max_element(temp_array_z, temp_array_z+4);
  minx = *std::min_element(temp_array_x, temp_array_x+4);
  miny = *std::min_element(temp_array_y, temp_array_y+4);
  minz = *std::min_element(temp_array_z, temp_array_z+4);
}


bool element::is_point_inside(node *point){
  bool isin = 0;

  if (    (this->minx <= point->x) && (this->maxx >= point->x) &&
          (this->miny <= point->y) && (this->maxy >= point->y) &&
          (this->minz <= point->z) && (this->maxz >= point->z) )             
  {

   return isin = PointInTetrahedron(nodelist[this->v1].cd_vec, nodelist[this->v2].cd_vec, nodelist[this->v3].cd_vec, nodelist[this->v4].cd_vec, point->cd_vec);
  }   
  else return 0;      
  // std::cout << *std::max_element(temp_array, temp_array+4);
  // std::cout <<"vals" << v1[j] << v2[j] << v3[j] << v4[j] << "\n";  
  // std::cout << in_bound;    
  //  isin = PointInTetrahedron(v1, v2, v3, v4, p);
  //std::cout << isin << "\n";
}

double element::interpret_pt_value(node *point){ 
  double N1, N2, N3, N4, V, V_01, V_02, V_03, V_04, mat11, mat12, mat13, mat41, mat21, mat22, mat23, mat42, mat31, mat32, mat33, mat43;
  double *v1c = nodelist[this->v1].cd_vec;
  double *v2c = nodelist[this->v2].cd_vec;
  double *v3c = nodelist[this->v3].cd_vec;
  double *v4c = nodelist[this->v4].cd_vec;
  double *p   = point->cd_vec;
  int x = 0, y = 1, z = 2;
  V_01 = (v2c[x]*((v3c[y]*v4c[z]) - (v4c[y]*v3c[z]))  +  v3c[x]*((v4c[y]*v2c[z]) - (v2c[y]*v4c[z]))  +  v4c[x]*((v2c[y]*v3c[z]) - (v3c[y]*v2c[z]))) / 6 ;
  V_02 = (v1c[x]*((v4c[y]*v3c[z]) - (v3c[y]*v4c[z]))  +  v3c[x]*((v1c[y]*v4c[z]) - (v4c[y]*v1c[z]))  +  v4c[x]*((v3c[y]*v1c[z]) - (v1c[y]*v3c[z]))) / 6 ;
  V_03 = (v1c[x]*((v2c[y]*v4c[z]) - (v4c[y]*v2c[z]))  +  v2c[x]*((v4c[y]*v1c[z]) - (v1c[y]*v4c[z]))  +  v4c[x]*((v1c[y]*v2c[z]) - (v2c[y]*v1c[z]))) / 6 ;
  V_04 = (v1c[x]*((v3c[y]*v2c[z]) - (v2c[y]*v3c[z]))  +  v2c[x]*((v1c[y]*v3c[z]) - (v3c[y]*v1c[z]))  +  v3c[x]*((v2c[y]*v1c[z]) - (v1c[y]*v2c[z]))) / 6 ;

  V = V_01 + V_02 + V_03 + V_04;


  mat11 = ((v4c[y]-v2c[y])*(v3c[z]-v2c[z])) - ((v3c[y]-v2c[y])*(v4c[z]-v2c[z]));
  mat21 = ((v3c[y]-v1c[y])*(v4c[z]-v3c[z])) - ((v3c[y]-v4c[y])*(v1c[z]-v3c[z]));
  mat31 = ((v2c[y]-v4c[y])*(v1c[z]-v4c[z])) - ((v1c[y]-v4c[y])*(v2c[z]-v4c[z]));
  mat41 = ((v1c[y]-v3c[y])*(v2c[z]-v1c[z])) - ((v1c[y]-v2c[y])*(v3c[z]-v1c[z]));

  mat12 = ((v3c[x]-v2c[x])*(v4c[z]-v2c[z])) - ((v4c[x]-v2c[x])*(v3c[z]-v2c[z]));
  mat22 = ((v4c[x]-v3c[x])*(v3c[z]-v1c[z])) - ((v1c[x]-v3c[x])*(v3c[z]-v4c[z]));
  mat32 = ((v1c[x]-v4c[x])*(v2c[z]-v4c[z])) - ((v2c[x]-v4c[x])*(v1c[z]-v4c[z]));
  mat42 = ((v2c[x]-v1c[x])*(v1c[z]-v3c[z])) - ((v3c[x]-v1c[x])*(v1c[z]-v2c[z]));


  mat13 = ((v4c[x]-v2c[x])*(v3c[y]-v2c[y])) - ((v3c[x]-v2c[x])*(v4c[y]-v2c[y]));
  mat23 = ((v3c[x]-v1c[x])*(v4c[y]-v3c[y])) - ((v3c[x]-v4c[x])*(v1c[y]-v3c[y]));
  mat33 = ((v2c[x]-v4c[x])*(v1c[y]-v4c[y])) - ((v1c[x]-v4c[x])*(v2c[y]-v4c[y]));
  mat43 = ((v1c[x]-v3c[x])*(v2c[y]-v1c[y])) - ((v1c[x]-v2c[x])*(v3c[y]-v1c[y]));

  N1 = (1/(6*V))*(6*V_01 + p[0]*mat11 + p[1]*mat12 + p[2]*mat13);
  N2 = (1/(6*V))*(6*V_02 + p[0]*mat21 + p[1]*mat22 + p[2]*mat23);
  N3 = (1/(6*V))*(6*V_03 + p[0]*mat31 + p[1]*mat32 + p[2]*mat33);
  N4 = (1/(6*V))*(6*V_04 + p[0]*mat41 + p[1]*mat42 + p[2]*mat43);
  
  /*
  cout << "Point Coordinates: " << p[0] << ", " << p[1] << ", " << p[2] << "\n";
  cout << "N1: " << N1 << "\n";
  cout << "N2: " << N2 << "\n";
  cout << "N3: " << N3 << "\n";
  cout << "N4: " << N4 << "\n\n";
  */

return ( (N1*nodelist[this->v1].temperature) + (N2*nodelist[this->v2].temperature) + (N3*nodelist[this->v3].temperature) + (N4*nodelist[this->v4].temperature) );
}

double dot_prod(double v_a[], double v_b[]){
  double product = 0;
  for (int i = 0; i < 3; i++)
  {
    product = product + (v_a[i] * v_b[i]);
  }
  return product;
}


void cross_prod(double cross_p[], double v_a[], double v_b[]){
    cross_p[0] = v_a[1] * v_b[2] - v_a[2] * v_b[1];
    cross_p[1] = v_a[2] * v_b[0] - v_a[0] * v_b[2];
    cross_p[2] = v_a[0] * v_b[1] - v_a[1] * v_b[0];
}


void vector_subtract(double subtr[], double v_a[], double v_b[]){
  for (int i = 0; i < 3; i++)
  {
    subtr[i] = v_a[i] - v_b[i];
  }
}


bool SameSide(double v1c[], double v2c[], double v3c[], double v4c[], double p[]){
    double dotV4, dotP;
    double cross_p[3];
    double * normal;
    //subtr_a and b are containers to temporarily hold the solutions of the vector subraction for use with the normal and dot calculations
    double subtr_a[3], subtr_b[3];

    vector_subtract(subtr_a, v2c, v1c);
    vector_subtract(subtr_b, v3c, v1c);
    cross_prod(cross_p, subtr_a, subtr_b);
    normal = cross_p;

    vector_subtract(subtr_a, v4c, v1c);
    dotV4 = dot_prod(normal, subtr_a);

    vector_subtract(subtr_a, p, v1c);
    dotP = dot_prod(normal, subtr_a);

    return (((dotV4 <= 0) && (dotP <= 0)) || ((dotV4 >= 0) && (dotP >= 0)));
}


bool PointInTetrahedron(double v1c[], double v2c[], double v3c[], double v4c[], double p[]){ 
	return SameSide(v1c, v2c, v3c, v4c, p) &&
           SameSide(v2c, v3c, v4c, v1c, p) &&
           SameSide(v3c, v4c, v1c, v2c, p) &&
           SameSide(v4c, v1c, v2c, v3c, p);               
}

/*
//Testing to check that vector_subtract is working properly
  double prod;
  double subtr_test[3];
  double v1[] = {6.9, -2.2, 4.32};
  double v2[] = {31.59, -9.867,15.113};
  vector_subtract(subtr_test, v1, v2);
  std::cout << subtr_test[0] << "," << subtr_test[1] << "," << subtr_test[2] << "\n";
  
  prod = dot_prod(v1, v2);
  std::cout << prod << "\n";
  
*/ 

bool get_FEM_vtu_data(std::string results_folder, int num_parts, int time_iter, std::string data_name_list, bool single_val, int pt_val){

  std::string line, line_comm;
  int nnodes,nel,offset_pt = 0, offset_el = 0;
  std::ostringstream ss;

  // iterate over mesh subdivision files
  for (int mesh_part = 0; mesh_part < num_parts; mesh_part++)
  {
    cout << "Importing data for time step " << time_iter << " ---- Part " << mesh_part << endl;
    ss << mesh_part;
    std::ifstream input((results_folder + "0/" +  ss.str() +  ".vtu").c_str());
	if (!input){std::cerr << "Error: certain VTU results files could not be opened! Skipping this time step \n\n"; return 0;}
    ss.str("");
    std::getline(input,line);
    std::getline(input,line);  
    std::getline(input,line);
    sscanf(line.c_str(),"<Piece NumberOfPoints=\"%d\"",&nnodes);
    ss << nnodes;
    sscanf(line.c_str(),("<Piece NumberOfPoints=\"" + ss.str() + "\" NumberOfCells=\"%d\">").c_str(),&nel);
    ss.str("");
     
    // Get node coordinates from the file and create node objects 
    double x, y, z, temperature;
    while (line.find("<DataArray") > 5000){
      std::getline(input,line);
    } 
    for (int k = 0 ; k < nnodes ; k++){
      std::getline(input,line);
      if ((!single_val) || (k + offset_pt) == pt_val){  
      sscanf(line.c_str(), "%lf %lf %lf", &x, &y, &z);
      node curr_node(x,y,z,k,offset_pt);
      nodelist.push_back(curr_node);      
      }
    }

    // Get element-node associations and create element objects     
    double v1,v2,v3,v4;
    while (line.find("<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">") > 5000){
      std::getline(input,line);
    }
    for (int k = 0 ; k < nel ; k++){
      std::getline(input,line);
      sscanf(line.c_str(), "%lf %lf %lf %lf", &v1, &v2, &v3, &v4);
      element curr_elem(v1, v2, v3, v4, k, offset_el, offset_pt);
      elementlist.push_back(curr_elem);
      //curr_elem.printinfo();
    }     
 
    // Get temperature point data and add to existing point objects
    while (line.find("<DataArray type=\"Float64\" Name=\"Solution\"") > 5000){
       std::getline(input,line);
    }
    
    for (int k = 0 ; k < nnodes ; k++){ 
      std::getline(input,line); 
      if ((!single_val) || (k + offset_pt) == pt_val){
        sscanf(line.c_str(), "%lf", &temperature);
        if (!single_val) {nodelist[k + offset_pt].add_temperature(temperature);}
        else cout << "ERROR __________________"; // {nodelist[0].add_temperature(temperature);}
       // nodelist[k + offset_pt].printinfo();
      }
    }
    
    // Close input vtu file and prepare for the next subdivision file (if more exist)
    input.close();
    offset_pt = offset_pt + nnodes;
    offset_el = offset_el + nel;
  }
  return 1;
}