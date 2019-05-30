#include "../System/Math.hpp"

unsigned long Math::random_cnt=0;
const float Math::PI=3.14159265f;
const float Math::TWO_PI = 2.f*PI;
const float Math::QUARTER_PI = PI/4.f;
const float Math::HALF_PI=PI/2.f;
const float Math::DEG_TO_RAD=PI/180.f;
const float Math::RAD_TO_DEG=180.f/PI;

float Math::rad_to_deg(float rad)
{
    return rad*RAD_TO_DEG;
}

double Math::rad_to_deg(double rad)
{
    return rad*RAD_TO_DEG;
}

float Math::deg_to_rad(float deg)
{
    return deg*DEG_TO_RAD;
}

double Math::deg_to_rad(double deg)
{
    return deg*DEG_TO_RAD;
}

double Math::fract(double val)
{
    return val>0.0 ? val - floor(val) : -(val - ceil(val));
}

float Math::fract(float val)
{
    return val>0.0 ? val - floor(val) : -(val - ceil(val));
}

double Math::rand(double n)
{
    return fract(sin(n) * 43758.5453123);
}

float Math::random(float min, float max)
{
    random_cnt++;
    if (random_cnt < 0) //overflow
        random_cnt = 0;
    return static_cast<float>((max-min)*rand(random_cnt+static_cast<double>(time(NULL)))+min);
}

double Math::random(double min, double max)
{
    random_cnt++;
    if (random_cnt < 0) //overflow
        random_cnt = 0;
    return (max - min)*rand(random_cnt + static_cast<double>(time(NULL))) + min;
}

int Math::random(int min, int max)
{
    random_cnt++;
    if (random_cnt < 0) //overflow
        random_cnt = 0;
    return static_cast<int>((max - min)*rand(random_cnt + static_cast<double>(time(NULL))) + min);
}
float Math::mix(float value1, float value2, float percent)
{
    return (value2 - value1)*percent + value1;
}

float Math::noise(float seed)
{
    float fl = std::floor(seed);
    return Math::mix(static_cast<float>(Math::rand(fl)), static_cast<float>(Math::rand(fl + 1.0f)), Math::fract(seed));
}

float Math::linear(float from, float to, float percentage)
{
    return from + ((to - from)*percentage);
}

float Math::map(float x, float in_min, float in_max, float out_min, float out_max)
{
    if ((in_max - in_min) >(out_max - out_min))
    {
        return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
    }
    else
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
}

float Math::limit(float value, float min, float max)
{
    return value > max ? max : (value < min ? min : value);
}

int Math::limitint(int value, int min, int max)
{
	return value > max ? max : (value < min ? min : value);
}

float Math::distance(sf::Vector2f vec1, sf::Vector2f vec2)
{
    return std::sqrt(std::pow(vec1.x - vec2.x, 2.f) + std::pow(vec1.y - vec2.y, 2.f));
}

bool Math::contains(sf::Vector2f pos, sf::Vector2f size, sf::Vector2f point)
{
    return pos.x<point.x && pos.x + size.x > point.x && pos.y < point.y && pos.y + size.y > point.y;
}

sf::Vector2f Math::screenToWorld(sf::Vector2f screen)
{
	const float y = ((screen.y / 64.0f) + ((screen.x) / 128.0f)) / 2.0f;
	return sf::Vector2f((screen.y / 64.0f) - y, y);
}

sf::Vector2f Math::worldToScreen(sf::Vector2f world)
{
	return sf::Vector2f((world.y - world.x) * 128.0f, (world.y + world.x) * 64.0f);
}

sf::Vector2i Math::worldToChunk(sf::Vector2f world)
{
	return sf::Vector2i(static_cast<int>(std::floor(world.x / 10.0f)), static_cast<int>(std::floor(world.y / 10.0f)));
}

float Math::sign(float value)
{
	return value >= 0.0f ? 1.0f : -1.0f;
}

sf::Vector2f Math::vec_mul(sf::Vector2f vec1, sf::Vector2f vec2)
{
    return sf::Vector2f(vec1.x*vec2.x, vec1.y*vec2.y);
}

sf::Vector2f Math::vec_div(sf::Vector2f vec1, sf::Vector2f vec2)
{
    return sf::Vector2f(vec1.x / vec2.x, vec1.y / vec2.y);
}

sf::Vector3f Math::rgb_to_hsv(sf::Vector3f rgb) //0-1 0-1 0-1
{
    //Normalize
    sf::Vector3f hsv;
    float _max = std::max(std::max(rgb.x, rgb.y), rgb.z);
    float _min = std::min(std::min(rgb.x, rgb.y), rgb.z);
    float delta = _max - _min;

    if(delta > 0)
    {
        if(_max == rgb.x)
        {
            hsv.x= 60.0f * static_cast<float>(std::fmod(((rgb.y - rgb.z) / delta), 6.0f));
        }
        else if(_max == rgb.y)
        {
            hsv.x= 60.0f * (((rgb.z - rgb.x) / delta) + 2.0f);
        }
        else if(_max == rgb.z)
        {
            hsv.x= 60.0f * (((rgb.x - rgb.y) / delta) + 4.0f);
        }

        if(_max > 0.0f)
        {
            hsv.y= delta / _max;
        }
        else
        {
            hsv.y= 0.0f;
        }

        hsv.z= _max;
    }
    else
    {
        hsv.x= 0.0f;
        hsv.y= 0.0f;
        hsv.z= _max;
    }

    if(hsv.x< 0.0f)
    {
        hsv.x= 360.0f + hsv.x;
    }
    return hsv; //0-360, 0-1, 0-1
}
sf::Vector3f Math::hsv_to_rgb(sf::Vector3f hsv) // 360-0 0-1 0-1
{
    float c = hsv.z * hsv.y; // Chroma
    float prime = static_cast<float>(std::fmod(hsv.x / 60.0f, 6.0f));
    float x = c * (1.0f - static_cast<float>(std::fabs(std::fmod(prime, 2.0f)) - 1.0f));
    float m = hsv.z - c;
    sf::Vector3f rgb;

    if(0.0f <= prime && prime < 1.0f)
    {
        rgb.x = c;
        rgb.y = x;
        rgb.z = 0.0f;
    }
    else if(1.0f <= prime && prime < 2.0f)
    {
        rgb.x = x;
        rgb.y = c;
        rgb.z = 0.0f;
    }
    else if(2.0f <= prime && prime < 3.0f)
    {
        rgb.x = 0.0f;
        rgb.y = c;
        rgb.z = x;
    }
    else if(3.0f <= prime && prime < 4.0f)
    {
        rgb.x = 0.0f;
        rgb.y = x;
        rgb.z = c;
    }
    else if(4.0f <= prime && prime < 5.0f)
    {
        rgb.x = x;
        rgb.y = 0.0f;
        rgb.z = c;
    }
    else if(5.0f <= prime && prime < 6.0f)
    {
        rgb.x = c;
        rgb.y = 0.0f;
        rgb.z = x;
    }
    else
    {
        rgb.x = 0.0f;
        rgb.y = 0.0f;
        rgb.z = 0.0f;
    }

    rgb.x += m;
    rgb.y += m;
    rgb.z += m;
    return rgb; //0-1 0-1 0-1
}

bool Math::triangle_contains(sf::Vector2f p,sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2)
{
    float A = 0.5f * (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y);
    float _sign = A < 0.0f ? -1.0f : 1.0f;
    float s = (p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y) * _sign;
    float t = (p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y) * _sign;

    return s > 0.0f && t > 0.0f && (s + t) < 2.0f * A * _sign;
}

bool Math::ray_segment_intersection(float x, float y, float dx, float dy, float x1, float y1, float x2, float y2, sf::Vector2f& result)
{
    float r, s, d;
    if (dy / dx != (y2 - y1) / (x2 - x1))
    {
        d = ((dx * (y2 - y1)) - dy * (x2 - x1));
        if (d != 0)
        {
            r = (((y - y1) * (x2 - x1)) - (x - x1) * (y2 - y1)) / d;
            s = (((y - y1) * dx) - (x - x1) * dy) / d;
            if (r >= 0 && s >= 0 && s <= 1)
            {
                result.x = x + r * dx;
                result.y = y + r * dy;
                return true;
            }
        }
    }
    return false;
}

sf::Vector2f Math::normalize(sf::Vector2f vector)
{
	const float angle = std::atan2(vector.x,vector.y);
	return sf::Vector2f(std::sin(angle),std::cos(angle));
}
float Math::distance_to_line(sf::Vector2f line1,sf::Vector2f line2,sf::Vector2f point)
{
	sf::Vector2f dir = line2 - line1;
	const float temp = dir.y;
	dir.y = -dir.x;
	dir.x = temp;
	const sf::Vector2f dirtopt = line1 - point;
	const sf::Vector2f norm = Math::normalize(dir);
	return std::fabs(norm.x*dirtopt.x+norm.y*dirtopt.y);
}


sf::Vector2f Math::lineIntersect(sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3)
{
    const float A1 = p1.y - p0.y;
    const float B1 = p0.x - p1.x;
    const float C1 = A1 * p0.x + B1 * p0.y;
    const float A2 = p3.y - p2.y;
	const float B2 = p2.x - p3.x;
    const float C2 = A2 * p2.x + B2 * p2.y;
    const float denominator = A1 * B2 - A2 * B1;

    return sf::Vector2f((B2 * C1 - B1 * C2) / denominator, (A1 * C2 - A2 * C1) / denominator);
}

