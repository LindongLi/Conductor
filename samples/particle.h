//
//  particle.h
//  Particles
//
//  Created by Lindong on 7/25/15.
//  Copyright (c) 2015 Lindong. All rights reserved.
//

#ifndef Particles_particle_h
#define Particles_particle_h

#include <list>
#include <stdlib.h>

class vec3
{
public:
    float x, y, z;
    
    vec3()
    {
        x = 0;
        y = 0;
        z = 0;
    }
    vec3(float a, float b, float c)
    {
        x = a;
        y = b;
        z = c;
    }
    void copy(vec3 &src)
    {
        x = src.x;
        y = src.y;
        z = src.z;
    }
};

class particle
{
public:
    vec3 position;
    vec3 direction;
    float speed;
    float size;
    int rgb_color[3];    //rgb color array
    float lifespan; //lifespan of a particle
    
    //Different types of constructors
    particle(vec3 &_position, float _size)
    {
        position.copy(_position);
        size = _size;
        rgb_color[0] = 255;
        rgb_color[1] = 0;
        rgb_color[2] = 0;
    }
    particle(vec3 &_position, vec3 &_direction, float _size, float _lifespan)
    {
        position.copy(_position);
        direction.copy(_direction);
        speed = 0.02;
        size = _size;
        rgb_color[0] = int(rand()%255);
        rgb_color[1] = int(rand()%255);
        rgb_color[2] = int(rand()%255);
        lifespan = _lifespan;
    }
    particle(vec3 &_position, vec3 &_direction, float _speed, float _size, float _lifespan)
    {
        position.copy(_position);
        direction.copy(_direction);
        speed = _speed;
        size = _size;
        rgb_color[0] = int(rand()%255);
        rgb_color[1] = int(rand()%255);
        rgb_color[2] = int(rand()%255);
        lifespan = _lifespan;
    }
    particle(vec3 &_position, vec3 &_direction, float _speed, float _size, float _lifespan, int *_color)
    {
        position.copy(_position);
        direction.copy(_direction);
        speed = _speed;
        size = _size;
        rgb_color[0] = _color[0];
        rgb_color[1] = _color[1];
        rgb_color[2] = _color[2];
        lifespan = _lifespan;
    }
    
    //Accessor Methods for Particle
    void getPosition(vec3 &_position)
    {
        _position.copy(position);
    }
    void getColor(int *_color)
    {
        _color[0] = rgb_color[0];
        _color[1] = rgb_color[1];
        _color[2] = rgb_color[2];
    }
    
    /*
     move - updates the position of the particle, updates the direction of the particle, rotation array
     and increments the age.
     */
    void move(float gravity, float floorSize, float friction)
    {
        direction.y -= gravity;
        position.x = position.x + direction.x * speed;
        position.y = position.y + direction.y * speed;
        position.z = position.z + direction.z * speed;
        
        if((position.x < -floorSize) || (position.x > floorSize)) {
            position.x = (position.x < -floorSize) ? -floorSize : floorSize;
            direction.x *= friction;
            direction.y *= friction;
            direction.z *= friction;
        }
        if(position.y < 0) {
            position.y = 0;
            direction.x *= friction;
            direction.y *= friction;
            direction.z *= friction;
        }
        if((position.z < -floorSize) || (position.z > floorSize)) {
            position.z = (position.z < -floorSize) ? -floorSize : floorSize;
            direction.x *= friction;
            direction.y *= friction;
            direction.z *= friction;
        }
        
        ++lifespan;
    }
};

#endif