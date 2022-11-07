#include "Definitions.h"
bool flag = false;
void euler_integration(float timestep, env_factors &factors)
{
    for (int i = 0; i < all_objects.size(); i++)
    {
        glm::vec3 old_ball_pos = all_objects[i].position;
        glm::vec3 old_ball_vel = all_objects[i].velocity;
        glm::vec3 new_velocity = all_objects[i].velocity + timestep * calculate_accel(all_objects[i], factors);
        glm::vec3 new_position = all_objects[i].position + (new_velocity) * timestep;
        float frac = 0.0f;
        int id = detect_collision_plane(new_position, all_objects[i].position, -1);
        flag = false;
        int count = 0;
        if (id > -1)
        {
                flag = true;
                frac = calculate_fraction(new_position, all_objects[i], id);

                old_ball_vel = all_objects[i].velocity;
                all_objects[i].velocity = all_objects[i].velocity + timestep * frac * calculate_accel(all_objects[i], factors);
                all_objects[i].position = all_objects[i].position + timestep * frac * (all_objects[i].velocity);

                all_objects[i].velocity = collision_response_plane(all_objects[i].velocity, id, factors.coeff_restitution, factors.coeff_friction);

                old_ball_vel = all_objects[i].velocity;
                all_objects[i].velocity = all_objects[i].velocity + (1.0f - frac) * timestep * calculate_accel(all_objects[i], factors);
                all_objects[i].position = all_objects[i].position + (1.0f - frac) * timestep * (all_objects[i].velocity);
        }
        else
        {
            all_objects[i].position = new_position;
            all_objects[i].velocity = new_velocity;
        }
        checkInsideDomain(all_objects[i]);

        for (int j = 0; j < all_objects.size(); j++)
        {
            if (i != j)
            {
                bool did_collide = detect_collision_ball(i,j);
                if (did_collide)
                    collision_response_ball(i, j, factors.coeff_restitution);
            }
        }
        checkInsideDomain(all_objects[i]);
    }
}

glm::vec3 calculate_accel(ball_struct& ball, env_factors& factors)
{
    glm::vec3 gravity = glm::vec3(0.0f,0.0f, -factors.g);
    glm::vec3 air_resistance = (-factors.d / ball.m) * (ball.velocity- factors.wind_velocity);
    glm::vec3 total_acc = gravity + air_resistance;
    if (glm::length(total_acc) < accel_tolerance)
        total_acc = glm::vec3(0.0f, 0.0f, 0.0f);
    return total_acc;
}

float calculate_fraction(glm::vec3& new_position, ball_struct& ball, int id)
{
    glm::vec3 location1 = new_position - glm::vec3(box[id*36+6], box[id * 36 + 6 +1], box[id * 36 + 6 +2]);
    float dis_below = abs(glm::dot(location1, faceNormals[id]));

    glm::vec3 location2 = ball.position - glm::vec3(box[id * 36 + 6], box[id * 36 + 6 +1], box[id * 36 + 6 +2]);
    float dis_above = abs(glm::dot(location2, faceNormals[id]));

    float frac = dis_above / (0.0001f+ dis_above + dis_below);
    return frac;
}

int detect_collision_plane(glm::vec3 new_position, glm::vec3 old_position, int id)
{
    float distance1 = 1, small_dist=1e100;
    float distance2 = 1;
    for (int i = 0; i < 6; i++)
    {
        glm::vec3 location_new = new_position - glm::vec3(box[i * 36 + 6], box[i * 36 + 6+1], box[i * 36 + 6+2]);
        glm::vec3 location_old = old_position - glm::vec3(box[i * 36 + 6], box[i * 36 + 6+1], box[i * 36 + 6+2]);
        distance1 = glm::dot(location_new, faceNormals[i]);
        float sign_distance1 = distance1 / (abs(distance1));
        distance2= glm::dot(location_old, faceNormals[i]);
        float sign_distance2 = distance2 / (abs(distance2));
        if ((sign_distance1 / sign_distance2) < 0 && (int)(id) != i)
        {
            return  i;
        }
    }
    return -1;
}

bool detect_collision_ball(int myID, int ball_ID)
{
    float distance1 = glm::length(all_objects[ball_ID].position - all_objects[myID].position);
    if (distance1 < 2 * br)
    {
        return true;
    }
    return false;
}

glm::vec3 collision_response_plane(glm::vec3& new_velocity, int id, float coeff_restitution, float friction)
{
    float old_vew_normal_vel_mag = glm::dot(faceNormals[id], new_velocity);
    glm::vec3 old_normal_vel = old_vew_normal_vel_mag * faceNormals[id];
    glm::vec3 new_normal_vel;
    
    if (glm::length(old_normal_vel) < velocity_tolerance)
        new_normal_vel = glm::vec3(0.0f, 0.0f, 0.0f);
    else
        new_normal_vel = old_normal_vel * coeff_restitution * -1.0f;

    glm::vec3 old_tanget_vel = new_velocity - old_normal_vel;
    glm::vec3 new_tangent_vel;
    if (glm::length(old_tanget_vel) < velocity_tolerance)
        new_tangent_vel = glm::vec3(0.0f, 0.0f, 0.0f);
    else
    {
        glm::vec3 dir = old_tanget_vel / glm::length(old_tanget_vel);
        new_tangent_vel = old_tanget_vel - glm::min(friction * glm::length(old_normal_vel), glm::length(old_tanget_vel)) * dir;
    }

    return (new_tangent_vel + new_normal_vel);
}

void collision_response_ball(int myID, int other_id, float coeff_restitution)
{
    if (!collision[myID][other_id])
    {
        float m1 = all_objects[myID].m;
        float m2 = all_objects[other_id].m;
        glm::vec3 dir = all_objects[other_id].position - all_objects[myID].position;
        glm::vec3 vel_diff = all_objects[other_id].velocity - all_objects[myID].velocity;

        float my_coeff = (2.0f * coeff_restitution *(m2 / (m1 + m2)))*((glm::dot(-vel_diff, -dir)) / ((float)glm::pow(glm::length(dir), 2)));
        float other_coeff = (2.0f * m1* coeff_restitution / (m1 + m2))*((glm::dot(vel_diff, dir)) / ((float)glm::pow(glm::length(dir), 2)));
        
        glm::vec3 my_vel = all_objects[myID].velocity - my_coeff * (-dir);
        glm::vec3 other_vel = all_objects[other_id].velocity - other_coeff * (dir);

        all_objects[myID].velocity = my_vel;
        all_objects[other_id].velocity = other_vel;
        collision[myID][other_id] = true;
    }
}


void checkInsideDomain(ball_struct& ball)
{
    float sign = 0;
    int myID = -1;
    float mydist = 1e100;
    for (int i = 0; i < 6; i++)
    {
        glm::vec3 loc1 = glm::dot(ball.position - glm::vec3(box[36 * i + 6], box[36*i+ 6+1], box[36 * i + 6 + 2]), faceNormals[i])*faceNormals[i];
        glm::vec3 loc2 = glm::dot(glm::vec3(0.0f, 0.0f, 0.0f) - glm::vec3(box[36 * i + 6], box[36 * i + 6 + 1], box[36 * i + 6 + 2]), faceNormals[i]) * faceNormals[i];
        sign = glm::dot(loc1 , loc2);
        if (sign < 0)
        {
            for (int j = 0; j < 6; j++)
            {
                loc1 = glm::dot(ball.position - glm::vec3(box[36 * i + 6*j], box[36 * i + 6*j + 1], box[36 * i + 6*j + 2]), faceNormals[i]) * faceNormals[i];
                loc2 = glm::dot(glm::vec3(0.0f, 0.0f, 0.0f) - glm::vec3(box[36 * i + 6*j], box[36 * i + 6*j + 1], box[36 * i + 6*j + 2]), faceNormals[i]) * faceNormals[i];
                sign = glm::dot(loc1, loc2);
                if (sign<0 && glm::length(loc1) < mydist)
                {
                    myID = i;
                    mydist = glm::length(loc1);
                }
            }
            break;
        }
    }
    glm::vec3 loc1 = glm::dot(ball.position - glm::vec3(box[36 * myID + 6], box[36 * myID + 6 + 1], box[36 * myID + 6 + 2]), faceNormals[myID]) * faceNormals[myID];

    glm::vec3 new_pos = (ball.position - glm::vec3(box[36 * myID + 6], box[36 * myID + 6 + 1], box[36 * myID + 6 + 2])) - loc1 + glm::vec3(box[36 * myID + 6], box[36 * myID + 6 + 1], box[36 * myID + 6 + 2]);

    ball.position = new_pos+ distance_tolerance*faceNormals[myID];
}