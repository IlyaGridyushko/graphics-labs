#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>

class VBO
{
public:
	// Reference ID of the Vertex Buffer Object
	GLuint ID;
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	VBO(GLfloat* vertices, GLsizeiptr size,GLenum usage);
    //default constructor
    VBO();
	// Binds the VBO
	void Bind();
	// Unbinds the VBO
	void Unbind();
	// Deletes the VBO
	void Delete();

    // Updates the VBO's data store
    void Data(GLfloat* vertices, GLsizeiptr size, GLenum usage);
    
	//GLfloat* Data(GLfloat* vertices, GLsizeiptr size);
};

#endif