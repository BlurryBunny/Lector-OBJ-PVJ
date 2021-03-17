#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glew.h> // put first GL libs
#include <glfw3.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>	// glm::translate, glm::rotate, glm::scale, glm::persective
#include <glm/gtc/type_ptr.hpp>

#include "shaders.hpp"
#include "files.h"


void Matrices();
int InitWindow();
void push_back_buffer(float value);
bool give_memory_buffer();
glm::vec3 calculate_normalFace(int stride_toNormal, int stride_toNextVer);
bool replace_Normal(glm::vec3 normal, int index, int stride, int n_vertex);
glm::vec3 vertexRes(glm::vec3 vf, glm::vec3	vi);
void printBuffer(int pos);
void printBufferFull();
bool change_light(char sense, int stride);
void attributeLoader(const GLfloat* g_vertex_buffer_data, int stride);

GLFWwindow* window;

glm::mat4x4 model, view, projection;

int width = 1200;
int height = 900;

float *buffer_obj;
int size_total;
int index_buffer;

//Light pos (this will use to diffuse and specular light) INITIAL LIGHT
glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, -5.0f);

int main()
{
	srand(time(NULL));
	if (InitWindow() < 0) return -1;

	//we use intial time when we need to calcule when the aplication use 1 second in real life.
	//double initialTime = glfwGetTime(); // not presition timer
	bool res = false;
	int frameCounter = 0;
	double totalTime = 0.0;
	int n_faces = 0;
	int unifLoc;
	// Id for program, Vetex array, Vertex buffer
	GLuint programID, vao, vbo;

	//Declarate an object type FILE.h
	File obj1 = File();
	//Give values and inicilizate the FILE objs
	obj1. setfileName("Models\\fishbowl.obj");
	if (!obj1.open()) {
		cout << "Error open the file" << endl;
	}else{
		obj1.saveInfo();
		//obj1.toString();
	}

	 //Give memory to the array
	 int stride = obj1.total_elements_Vertex();
	 size_total = obj1.get_size_bufferIdVer() * stride;
	 
	 if(!give_memory_buffer()){
		 cout << "Error give memory to buffer" << endl;	
	 }

	 index_buffer = 0;
	 int counter_faces = 0;
	for (int i = 0; i < obj1.get_size_bufferIdVer(); i++)
	{
		//Check if we have a new face to calculate the normal 
		if (counter_faces < 3) {
			counter_faces++;
		}
		else {
			//replace_Normal(vec3,index to the value 'x' of  last normal face saved)
			//calculate_normalFace(index to the 'x' of the last normal vertex saved, last position saved of the buffer)

			//cout << buffer_obj[((i + 1) * stride) - 6] << " " << buffer_obj[((i + 1) * stride) - 5] << " " << buffer_obj[((i + 1) * stride) - 4] << endl;
			//cout << endl;
			
			glm::vec3 normalFace = calculate_normalFace((i + 1) * stride - 12, stride);
			//cout << normalFace[0] << " " << normalFace[0] << " " << normalFace[0] << " " << endl;
			if (replace_Normal(normalFace, ((i + 1) * stride) - 4, stride, 3) != true) {
				cout << "Error calculate and replace normal" << endl;
				//exit(0);
			}
			
			counter_faces = 0;
		}

		glm::vec3 v = obj1.getVertex(obj1.getIdVertex(i)-1); //Get the value of x,y,z of a vertex indicates on a face of the obj
		glm::vec3 vn = obj1.getNormalVertex(obj1.getIdNormalVertex(i)-1);

		
		//cout << "new" << endl;	cout << "value: " << v[o] << endl;
		for (int o = 0; o < 3; o++) { push_back_buffer(v[o]); }	//Vertex Pos
		for (int p = 0; p < 3; p++) { push_back_buffer(vn[p]);}	//Normal Pos
		for (int q = 0; q < 3; q++) {							//Color RGB
			//push_back_buffer(1.0f/float(rand() % 10 + 1));	//rand color
			push_back_buffer(1.0f / float(q+1));				//cream color
			//push_back_buffer(0.5f);							//gray color.

		}
		for (int r = 0; r< 3; r++) { push_back_buffer(0.0f); }	//Normal Face
		for (int s = 0; s < 3; s++) { push_back_buffer(lightPos[s]);} //Light pos 
	}
	
	/*buff_obj;
	Vertice		Normal		Color		NormalFace
	[x,y,z]		[x,y,z]		[r,g,b]		  [x,y,z]
	*/

	//Load the shaders using the .glsl file
	programID = LoadShaders("Shaders\\vs_ilum_flat.glsl","Shaders\\fs_ilum_flat.glsl");

	//VAO
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	//VBO 
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	n_faces = obj1.get_size_bufferIdVer()/3;
	//main loop
	do
	{
		char sense;
		
		//check if a key was press

		/*Change shaders*/
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			programID = LoadShaders("Shaders\\vs_ilum_flat.glsl", "Shaders\\fs_ilum_flat.glsl");
		}

		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			programID = LoadShaders("Shaders\\vs_ilum_phong.glsl", "Shaders\\fs_ilum_phong.glsl");
		}

		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
			programID = LoadShaders("Shaders\\vs_ilum_gourand.glsl", "Shaders\\fs_ilum_gourand.glsl");
		}

		//movement of the diffuse light 
		/*UP*/
		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && !res) {
			res = change_light('u',stride);
		}
		/*DOWN*/
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && !res) {
			res = change_light('d', stride);
		}
		/*RIGHT*/
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !res) {
			res = change_light('r', stride);
		}
		/*LEFT*/
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && !res) {
			res = change_light('l', stride);
		}

		const GLfloat* g_vertex_buffer_data = buffer_obj;
		attributeLoader(g_vertex_buffer_data, stride);

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
			res = false;	//res return false if a second has passed.
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
	// rotate(previous model, angle to rotate on radians, vec3 indicates x, y, z / 0 - 1.0 to rotate)
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f));
	//model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
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

glm::vec3 calculate_normalFace( int stride_toNormal, int stride_toNextVer) {

	glm::vec3 v3 = glm::vec3(buffer_obj[stride_toNormal], buffer_obj[stride_toNormal +1], buffer_obj[stride_toNormal +2]);
	stride_toNormal = stride_toNormal - stride_toNextVer;
	glm::vec3 v2 = glm::vec3(buffer_obj[stride_toNormal], buffer_obj[stride_toNormal + 1], buffer_obj[stride_toNormal + 2]);
	stride_toNormal = stride_toNormal - stride_toNextVer;
	glm::vec3 v1 = glm::vec3(buffer_obj[stride_toNormal], buffer_obj[stride_toNormal + 1], buffer_obj[stride_toNormal + 2]);

	glm::vec3 res = glm::cross(vertexRes(v1,v2),vertexRes(v1,v3));
	return res;
}

glm::vec3 vertexRes(glm::vec3 vf, glm::vec3	vi) {
	return glm::vec3(vf[0]-vi[0], vf[1]-vi[0], vf[2]-vi[2]);
}
bool replace_Normal(glm::vec3 normal, int index, int stride, int n_vertex ) {
	bool res = true;
	for (int i = 0; i < n_vertex && res; i++) {
		if (index < size_total && index >= 0) {
			buffer_obj[index] = normal[2];
			buffer_obj[index - 1] = normal[1];
			buffer_obj[index - 2] = normal[0];
			index -= stride;
		}
		else {
			res = false;
		}
	}
	return res;
}
void printBuffer(int pos) {
	for (int i = pos; i < index_buffer; i++)
	{
		cout <<buffer_obj[i] << " ";
	}
	cout << endl;
}

void printBufferFull() {
	int counter = 0;
	for (int i = 0; i < index_buffer; i++)
	{
		
		if (counter == 15) {
			cout << endl;
			counter = 0;
		}
		counter++;
		cout<< buffer_obj[i] <<" ";
	}
}

bool change_light(char sense, int stride) {

	bool res = true;
	switch (sense) {
	case 'u':
		lightPos[1] = lightPos[1] + 0.5f;
		break;
	case 'd':
		lightPos[1] = lightPos[1] - 0.5f;
		break;
	case 'r':
		lightPos[0] = lightPos[0] + 0.5f;
		break;
	case 'l':
		lightPos[0] = lightPos[0] - 0.5f;
		break;
	default:
		res = false;
		break;
	}

	
	/*cout << "se presiono una tecla" << endl;
	cout << lightPos[0] << " " << lightPos[1] << " " << lightPos[2] << " \n" << endl;*/
	// each 12 elements we found the x coordinate of the light pos
	//actual stride is 15
	int counter = 0;
	for (int i = 0; i < index_buffer; i++)
	{
		// stride -> n-1
		if (counter == stride ) {
			buffer_obj[i - 3] = lightPos[0];
			buffer_obj[i - 2] = lightPos[1];
			counter = 0;
		}
		counter++;
	}

	return res;
}

void attributeLoader(const GLfloat* g_vertex_buffer_data, int stride) {

	//This take the tree values of the array like the vertex to triangulate
	//offset of 3 floats (12 bytes)
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * size_total, g_vertex_buffer_data, GL_STATIC_DRAW);

	//So we need to separate the vertex (X,Y,Z) and color (RGB) values. 

	//glVertexAttribPointer(location shader, elements to save, kind of value , polarize , next begin since it count the strite ); 

	//Vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * stride, (void*)0);
	glEnableVertexAttribArray(0);
	//Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * stride, (void*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);
	//Color
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * stride, (void*)(sizeof(GLfloat) * 6));
	glEnableVertexAttribArray(2);
	//normal Face
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * stride, (void*)(sizeof(GLfloat) * 9));
	glEnableVertexAttribArray(3);
	//Light pos
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * stride, (void*)(sizeof(GLfloat) * 12));
	glEnableVertexAttribArray(4);
}