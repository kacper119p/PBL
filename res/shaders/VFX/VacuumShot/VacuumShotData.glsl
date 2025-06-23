struct particle {
    vec3 position;
    float scale;
    vec3 velocity;
    float life;
};

const float MAX_LIFETIME = 2.0;
const float MAX_VELOCITY = 25.0;
const float DRAG = 5.0;
const float START_SCALE = 0.5;
