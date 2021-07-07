out vec4 vert_pos;
out vec4 color;

void main()
{
    // transform the vertex position
	vert_pos = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;	

    // transform the texture coordinates
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

    // forward the vertex color
    color = gl_Color;
    gl_FrontColor = gl_Color;
}