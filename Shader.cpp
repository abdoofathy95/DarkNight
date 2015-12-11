#include "Shader.h"


// Method to load the shader contents from a string
void Shader::loadAndCompileVertexShader(const string &filename,int programId)
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	std::ifstream file;

	file.open(filename.c_str());

	if (!file.good())
	{
		exit(-1);
	}

	// Create a string stream
	std::stringstream stream;

	// Dump the contents of the file into it
	stream << file.rdbuf();

	// Close the file
	file.close();

	// Convert the StringStream into a string
	string source = stream.str();

	// Get the source string as a pointer to an array of characters
	const char *sourceChars = source.c_str();

	// Associate the source with the shader id
	glShaderSource(vertexShader,1,&sourceChars,0);
	glCompileShader(vertexShader);
	glAttachShader(programId, vertexShader);
}

void Shader::loadAndCompileFragShader(const string &filename, int programId)
{
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::ifstream file;

	file.open(filename.c_str());

	if (!file.good())
	{
		exit(-1);
	}

	// Create a string stream
	std::stringstream stream;

	// Dump the contents of the file into it
	stream << file.rdbuf();

	// Close the file
	file.close();

	// Convert the StringStream into a string
	string source = stream.str();

	// Get the source string as a pointer to an array of characters
	const char *sourceChars = source.c_str();

	// Associate the source with the shader id
	glShaderSource(fragmentShader, 1, &sourceChars, 0);
	glCompileShader(fragmentShader);
	glAttachShader(programId, fragmentShader);
}