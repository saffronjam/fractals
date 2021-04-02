#version 430

uniform dvec2 juliaC;
uniform dvec2 fractalTL;
uniform double xScale;
uniform double yScale;
uniform int iterations;

out vec4 pixelColor;

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
    dvec2 id = gl_FragCoord.xy;

	dvec2 scaledOffset = dvec2(id.x * xScale, id.y * yScale );
	dvec2 fractalCoord = dvec2(fractalTL + scaledOffset);

	dvec2 z = fractalCoord;
	dvec2 c = dvec2(juliaC.x, -juliaC.y);

	int n = 0;
	for (; n < iterations && dot(z, z) < 4.0; n++)
	{
        double zRealTmp = z.x;
        z.x = (zRealTmp * zRealTmp) - (z.y * z.y) + c.x;
        z.y = 2.0 * zRealTmp * z.y + c.y;
	}

	pixelColor = vec4(n, 0.0, 0.0, 1.0);
}
