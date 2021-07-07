#version 430

layout(rgba32f, binding = 0) uniform image2D img_data;
layout(rgba32f, binding = 1) uniform image2D img_palette;

uniform float maxPixelValue;
uniform int paletteWidth;

out vec4 pixelColor;

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
    dvec2 id = gl_FragCoord.xy;
	float final = map(imageLoad(img_data, ivec2(id)).x, 0.0, maxPixelValue, 0.0, paletteWidth - 1);
	pixelColor = imageLoad(img_palette, ivec2(clamp(final, 0, paletteWidth), 0));
}
