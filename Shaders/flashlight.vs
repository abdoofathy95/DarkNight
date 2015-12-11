varying vec3 position;
varying vec3 normal;

void main()
{
    vec4 eyeCoordPos = gl_ModelViewMatrix * gl_Vertex;

    position = eyeCoordPos.xyz / eyeCoordPos.w;
    normal = normalize(gl_NormalMatrix * gl_Normal);

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;    
}