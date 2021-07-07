#define MAX_LIGHTS 128

in vec4 vert_pos;

uniform sampler2D texture;
uniform vec2 lightPositions[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform float lightRadiuses[MAX_LIGHTS];
uniform int nLights;
uniform	vec3 ambient;

uniform vec2 winSize;

void main()
{
    vec4 lightSummary = vec4(0.0, 0.0, 0.0, 1);

    for(int i = 0; i < nLights; i++)
	{
        //Convert light to view coords
        lightPositions[i] = (gl_ModelViewProjectionMatrix * vec4(lightPositions[i], 0, 1)).xy;
        //Calculate the vector from light to pixel (Make circular)
        vec2 lightToFrag = lightPositions[i] - vert_pos.xy;
        lightToFrag.y = lightToFrag.y / 1.78;
        //Length of the vector (distance)
        float vecLength = clamp(length(lightToFrag) * winSize.x / lightRadiuses[i] / 2.0, 0.0, 1.0);

        lightSummary += vec4(lightColors[i] * (1.0 - vecLength), 1.0);
    }

    // lookup the pixel in the texture
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    // multiply it by the color and lighting
	gl_FragColor = gl_Color * pixel * (clamp(vec4(ambient, 1.0f) + lightSummary, 0.0, 1.0));
}


