#pragma once
#  include <GL/glew.h>
#  include <fstream>
#  include <sstream>

using namespace std;
class Shader
{
public:
	void loadAndCompileVertexShader(const string &filename, int programId);
	void loadAndCompileFragShader(const string &filename, int programId);
};

