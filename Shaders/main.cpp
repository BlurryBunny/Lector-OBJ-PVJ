#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h> // put first GL libs
#include <glfw3.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>	// glm::translate, glm::rotate, glm::scale, glm::persective
#include <glm/gtc/type_ptr.hpp>

#include "shaders.hpp"
#include "files.h"

typedef struct {
	float x;
	float y;
	float z;
}Vertex;

void Matrices();
int InitWindow();
void push_back_buffer(float value);
bool give_memory_buffer();

GLFWwindow* window;

glm::mat4x4 model, view, projection;

int width = 1200;
int height = 900;

float *buffer_obj;
int size_total;
int index_buffer;
int main()
{

	if (InitWindow() < 0) return -1;

	//we use intial time when we need to calcule when the aplication use 1 second in real life.
	//double initialTime = glfwGetTime(); // not presition timer
	int frameCounter = 0;
	double totalTime = 0.0;
	int n_faces = 0;
	int unifLoc;
	// Id for program, Vetex array, Vertex buffer
	GLuint programID, vao, vbo;

	//Declarate an object type FILE.h
	File obj1 = File();

	//Give values and inicilizate the FILE objs
	obj1.fileName("Models\\Pecera_Mia_10.obj");
	if (!obj1.open()) {
		cout << "Error open the file" << endl;
	}else{
		obj1.saveInfo();
		//obj1.toString();
	}

	//Give memory to the array
	 size_total = obj1.get_size_bufferIdVer() * (obj1.total_elements_Vertex());
	 
	 if(!give_memory_buffer()){
		 cout << "Error give memory to buffer" << endl;	
	 }

	 index_buffer = 0;
	for (int i = 0; i < obj1.get_size_bufferIdVer(); i++)
	{
		
		vector<float>v = obj1.getVertex(obj1.getIdVertex(i)-1); //Get the value of x,y,z of a vertex indicates on a face of the obj
		vector<float>vn = obj1.getNormalVertex(obj1.getIdVertex(i)-1);

		
		for (int o = 0; o < v.size(); o++){ push_back_buffer(v[o]);}
		for (int p = 0; p < vn.size(); p++) { push_back_buffer(vn[p]);}
		for (int q = 0; q < 3; q++) { push_back_buffer(1.0f/float(q+1)); }

		int size_per_vertex = v.size() + vn.size() + 3;
	}
	

	/*buff_obj;
	Vertice		Normal		Color
	[x,y,z]		[x,y,z]		[r,g,b]
	*/


	//Load the shaders using the .glsl file
	programID = LoadShaders("Shaders\\vs_basico_proyect.glsl","Shaders\\fs_basico_proyect.glsl");

	//VAO
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	//VBO 
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	const GLfloat* g_vertex_buffer_data = buffer_obj;

	//This take the tree values of the array like the vertex to triangulate
	//offset of 3 floats (12 bytes)
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * size_total, g_vertex_buffer_data, GL_STATIC_DRAW);
	
	//So we need to separate the vertex (X,Y,Z) and color (RGB) values. 

	//glVertexAttribPointer(location shader, elements to save, kind of value , polarize , next begin since it count the strite ); 

	//Vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void*)0);
	glEnableVertexAttribArray(0);
	//Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void*)(sizeof(GLfloat)*3));
	glEnableVertexAttribArray(1);
	//Color
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void*)(sizeof(GLfloat)*6));
	glEnableVertexAttribArray(2);
	

	n_faces = obj1.get_size_bufferIdVer()/3;
	//principal loop
	do
	{
		double initialTime = glfwGetTime(); // not a presition timer

		// a presition timer use the inside clock of the computer with ticks and frecuency
		// the problem of that timers it depends complety of the operative system.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//Shaders
		glUseProgram(programID); //The ID use the fragment and vertex shader
		
		//Use Matrix to view, transform and proyect the objects.
		Matrices();	//Generate each frame the point of view.

		unifLoc = glGetUniformLocation(programID, "model");
		glUniformMatrix4fv(unifLoc, 1, GL_FALSE, glm::value_ptr(model));

		unifLoc = glGetUniformLocation(programID, "view");
		glUniformMatrix4fv(unifLoc, 1, GL_FALSE, glm::value_ptr(view));

		unifLoc = glGetUniformLocation(programID, "projection");
		glUniformMatrix4fv(unifLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//get location of variables
		int location = glGetUniformLocation(programID, "color");
		glUniform3f(location, 1.0f, 0.0f, 1.0f); // write in the position the values on the function.

		//draw the shaders
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, n_faces * 3); //Termina la configuracion del shader y esta listo para dibujar.

		glfwSwapBuffers(window);
		glfwPollEvents();

		//GAME

		//Paints

		//Frame Rate
		double ts = 1.0f / 60.0f;
		double diff;
		double finalTime = glfwGetTime(); // not presition timer

		diff = finalTime - initialTime;
		while (true) {
			if (diff >= ts){break;}
			finalTime = glfwGetTime();
			diff = finalTime - initialTime;
		}

		//update total frames and the timer acoording to the limit time
		frameCounter++;
		totalTime += diff;

		//show results
		//printf("%f\n", diff); 
		if (totalTime >= 1.0) {
			//printf("Frame rate per second limit to: %d\n",frameCounter);
			frameCounter = 0;
			totalTime = 0.0;
		}


	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glfwTerminate();

	return 1;
}

// open sources to show something in a new window.
int InitWindow(void)
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Test", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	//Glew
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	//glfwSetMouseButtonCallback(window, mouse_button_callback);

	//OpenGl
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glViewport(0, 0, 1000, 1000);
	glEnable(GL_DEPTH_TEST);

	return 1;
}

float angle = 0.0f;
void Matrices() {
	//in this case we rotate the objects 
	//glRotate(Matrix, grades to rotate, select axes)
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 1.0f));
	(angle < 360.0f) ? angle += 1.0f : angle = 1.0f;
	//lookat(eye, center, vector to point);
	view = glm::lookAt(glm::vec3(0.0f, 0.0f, -15.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
}

bool give_memory_buffer() {
	bool res = true;
	buffer_obj = (float*)malloc(sizeof(float)* size_total);
	return res;
}

void push_back_buffer(float value) {
	if (index_buffer <= size_total) { buffer_obj[index_buffer] = value; index_buffer++; }
}