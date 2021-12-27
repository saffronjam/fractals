#version 430

#define PI (3.14159265359)

double atan_dp(double y, double x)
{
    const double atan_tbl[] = {
    -3.333333333333333333333333333303396520128e-1LF,
     1.999999117496509842004185053319506031014e-1LF,
    -1.428514132711481940637283859690014415584e-1LF,
     1.110012236849539584126568416131750076191e-1LF,
    -8.993611617787817334566922323958104463948e-2LF,
     7.212338962134411520637759523226823838487e-2LF,
    -5.205055255952184339031830383744136009889e-2LF,
     2.938542391751121307313459297120064977888e-2LF,
    -1.079891788348568421355096111489189625479e-2LF,
     1.858552116405489677124095112269935093498e-3LF
    };

    double ax = abs(x);
    double ay = abs(y);
    double t0 = max(ax, ay);
    double t1 = min(ax, ay);
    
    double a = 1 / t0;
    a *= t1;

    double s = a * a;
    double p = atan_tbl[9];

    p = fma( fma( fma( fma( fma( fma( fma( fma( fma( fma(p, s,
        atan_tbl[8]), s,
        atan_tbl[7]), s, 
        atan_tbl[6]), s,
        atan_tbl[5]), s,
        atan_tbl[4]), s,
        atan_tbl[3]), s,
        atan_tbl[2]), s,
        atan_tbl[1]), s,
        atan_tbl[0]), s*a, a);

    double r = ay > ax ? (1.57079632679489661923LF - p) : p;

    r = x < 0 ?  3.14159265358979323846LF - r : r;
    r = y < 0 ? -r : r;

    return r;
}

double sin_dp(double x)
{
    const double a3 = -1.666666660646699151540776973346659104119e-1LF;
    const double a5 =  8.333330495671426021718370503012583606364e-3LF;
    const double a7 = -1.984080403919620610590106573736892971297e-4LF;
    const double a9 =  2.752261885409148183683678902130857814965e-6LF;
    const double ab = -2.384669400943475552559273983214582409441e-8LF;

    const double m_2_pi = 0.636619772367581343076LF;
    const double m_pi_2 = 1.57079632679489661923LF;

    double y = abs(x * m_2_pi);
    double q = floor(y);
    int quadrant = int(q);

    double t = (quadrant & 1) != 0 ? 1 - y + q : y - q;
    t *= m_pi_2;

    double t2 = t * t;
    double r = fma(fma(fma(fma(fma(ab, t2, a9), t2, a7), t2, a5), t2, a3),
        t2*t, t);

    r = x < 0 ? -r : r;

    return (quadrant & 2) != 0 ? -r : r;
}

double cos_dp(double x)
{
    return sin_dp(x + 1.57079632679489661923LF);
}

double length_dp(dvec2 a) {
    return sqrt(a.x * a.x + a.y * a.y);
}

double pow_dp(double a, double n) {
    double result = a;
    highp int exp = highp int(n);
    for(int i = 0; i < exp; i++){
        result = result * result;
    }
    return result;
}

//dvec2 cx_pow(dvec2 aD, double nD) {
//    vec2 a = vec2(aD.x, aD.y);
//    float n = float(nD);
//
//    float angle = atan(a.y, a.x);
//    float r = length(a);
//    float real = pow(r, n) * cos(n*angle);
//    float im = pow(r, n) * sin(n*angle);
//    return dvec2(real, im);
//}

dvec2 cx_pow(dvec2 a, double n) {
    double angle = atan_dp(a.y, a.x);
    double r = length_dp(a);


    // The abcence of pow dp float makes us unable to do as high res as normal mandelbrot :(
    double powRes = double(pow(highp float(r), highp float(n)));

    double real = powRes * cos_dp(n*angle);
    double im = powRes * sin_dp(n*angle);
    return dvec2(real, im);
}


uniform dvec2 fractalTL;
uniform double xScale;
uniform double yScale;
uniform int iterations;

uniform dvec4 constants;
uniform dvec4 exponents;

out vec4 pixelColor;

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

dvec2 calc(dvec2 old, dvec2 c)
{
    dvec2 new = dvec2(0, 0);
    new = new + cx_pow(old, exponents.x) * constants.x;
    new = new + cx_pow(old, exponents.y) * constants.y;
    new = new + cx_pow(old, exponents.y) * constants.z;
    new = new + cx_pow(old, exponents.w) * constants.w;
    return new;
}

void main()
{
    dvec2 id = gl_FragCoord.xy;

	dvec2 scaledOffset = dvec2(id.x * xScale, id.y * yScale );
	dvec2 fractalCoord = dvec2(fractalTL + scaledOffset);

    dvec2 z = fractalCoord;
	dvec2 c = fractalCoord;

	int n = 0;
	for (; n < iterations && dot(z, z) < 4.0; n++)
	{
        z = calc(z, c);
	}
	
	pixelColor = vec4(n, 0.0, 0.0, 1.0);
}
