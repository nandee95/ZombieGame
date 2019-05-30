#pragma once

#include <math.h>
#include <ctime>
#include <SFML\Graphics.hpp>

class Math
{
private:
    static unsigned long random_cnt;
public:
    static const float PI;
    static const float TWO_PI;
	static const float HALF_PI;
	static const float QUARTER_PI;
    static const float RAD_TO_DEG;
    static const float DEG_TO_RAD;

    static double rad_to_deg(double rad);
    static float rad_to_deg(float rad);
    static float deg_to_rad(float deg);
    static double deg_to_rad(double deg);

    static sf::Vector3f rgb_to_hsv(sf::Vector3f rgb);
    static sf::Vector3f hsv_to_rgb(sf::Vector3f hsv);

    //TODO change raw value to sf::Vector2f
    static bool ray_segment_intersection(float x, float y, float dx, float dy, float x1, float y1, float x2, float y2, sf::Vector2f& result);

    static bool triangle_contains(sf::Vector2f p,sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2);

	static sf::Vector2f normalize(sf::Vector2f vector);
    static float distance_to_line(sf::Vector2f line1,sf::Vector2f line2,sf::Vector2f point);
    static sf::Vector2f lineIntersect(sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3);

    static float fract(float val);
    static double fract(double val);

    static double rand(double n);

    static float random(float min, float max);
    static double random(double min, double max);
    static int random(int min, int max);

    static float mix(float value1, float value2, float percent);

    static float noise(float seed);

    static float linear(float from, float to, float percentage);

    static float map(float x, float in_min, float in_max, float out_min, float out_max);
	static float limit(float value, float min, float max);
	static int limitint(int value, int min, int max);

    static float distance(sf::Vector2f vec1, sf::Vector2f vec2);
    static bool contains(sf::Vector2f pos, sf::Vector2f size, sf::Vector2f point);

	static sf::Vector2f screenToWorld(sf::Vector2f screen);

	static sf::Vector2f worldToScreen(sf::Vector2f world);
	static sf::Vector2i worldToChunk(sf::Vector2f world);

	static float sign(float value);

    static sf::Vector2f vec_mul(sf::Vector2f vec1, sf::Vector2f vec2);
    static sf::Vector2f vec_div(sf::Vector2f vec1, sf::Vector2f vec2);
};
