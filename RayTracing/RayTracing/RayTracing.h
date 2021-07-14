#ifndef __ray_tracing_h__
#define __ray_tracing_h__

#include <list>
#include "objects.h"

#define maxRefDepth 0

class scene {
private:
	int windowWidth = 1280, windowHeight = 720;
	vec3 cam, screenCenter;

	std::list<object*> objects;
	std::list<light*> lights;

	vec3 GetRefl(const vec3& vec, const vec3& normal) const;
	vec3 RayCast(ray &ray, int currentDepth) const;
public:
	scene(const vec3& cam, const vec3& screenCenter, const std::list<object*>& objects, const std::list<light*>& lights);
	scene(int width, int height, const vec3& cam, const vec3& screenCenter, const std::list<object*>& objects, const std::list<light*>& lights);

	void render() const;
};

#endif
