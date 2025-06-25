struct particle {
    vec3 position;
    float scale;
    vec3 velocity;
    float life;
    float rotation;
    float[3] padding;
};


const float MAX_LIFETIME = 1.5;
const float MAX_VELOCITY = 0.5;
const float DRAG = 0.5;
const float MAX_SCALE = 1.5;
const int SPRITE_COLUMNS = 8;
const int SPRITE_ROWS = 8;
const int TOTAL_FRAMES = SPRITE_COLUMNS * SPRITE_ROWS;
const float FRAME_DURATION = MAX_LIFETIME / float(TOTAL_FRAMES);
