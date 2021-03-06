#include "camera_control_glfw.hpp"

#include "vcl/core/scene/picking/picking.hpp"

namespace vcl
{


enum class camera_action_state {none, translation, rotation, scale, translation_depth, draw_curve};

void camera_control_glfw::update_mouse_move(camera_scene& camera, GLFWwindow* window, float x1, float y1, std::vector<vec3> &draw_points)
{
    assert(window!=nullptr);

    // Get type of click (left, right), is CTRL pressed
    const bool mouse_click_left  = (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT )==GLFW_PRESS);
    const bool mouse_click_right = (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS);
    const bool key_ctrl = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL));
    const bool key_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT));
    const bool key_space = (glfwGetKey(window, GLFW_KEY_SPACE));

    if(update==false || key_shift)
    {
        x0 = x1;
        y0 = y1;
        return;
    }

    // Get window size
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    const float w = static_cast<float>(width);
    const float h = static_cast<float>(height);

    // Set action state accordingly
    camera_action_state state = camera_action_state::none;

    if(!mouse_click_left && !mouse_click_right)
        state = camera_action_state::none;
    else if( mouse_click_left && !key_ctrl && !key_space)
        state = camera_action_state::rotation;
    else if( mouse_click_left && key_ctrl )
        state = camera_action_state::translation;
    else if( mouse_click_right && !key_ctrl )
        state = camera_action_state::scale;
    else if( mouse_click_right && key_ctrl )
        state = camera_action_state::translation_depth;
    else if( mouse_click_left && key_space)
        state = camera_action_state::draw_curve;

    // ************************************************* //
    // Compute transformation to apply on the camera
    // ************************************************* //
    if( state == camera_action_state::translation ) {

        const float tx =  (x1-x0)/w;
        const float ty = -(y1-y0)/h;
        camera.apply_translation_in_screen_plane( tx, ty );

    }

    else if( state == camera_action_state::rotation ) {

        // relative position on screen
        const float ux0 = 2*x0/float(w)-1;
        const float uy0 = 1-2*y0/float(h);

        const float ux1 = 2*x1/float(w)-1;
        const float uy1 = 1-2*y1/float(h);

        // apply rotation using trackball method
        camera.apply_rotation_trackball(ux0, uy0, ux1, uy1);
    }

    else if ( state == camera_action_state::scale ) {

        const float scale_magnitude = (y0-y1)/h;

        camera.apply_scaling( scale_magnitude );
    }

    else if ( state == camera_action_state::translation_depth ) {
        const float translation_magnitude = (y1-y0)/h;

        camera.apply_translation_orthogonal_to_screen_plane( translation_magnitude );
    }

    else if( state == camera_action_state::draw_curve ){
        if(reset_stroke){
            draw_points.clear();
            reset_stroke = false;
        }
        const float ux1 = 2*x1/float(w)-1;
        const float uy1 = 1-2*y1/float(h);
        vcl::ray r = picking_ray(camera, ux1, uy1);
        vcl::vec3 n = vcl::normalize(camera.orientation * vcl::vec3(0.0f,0.0f,-1.0f));
        float t = -vcl::dot(r.p, n) / vcl::dot(r.u, n); //solution in the plane of normal opposed to the viewpoint and passing through the origin
        vcl::vec3 p =  r.p + t * r.u ;
        draw_points.push_back(p);
    }

    else if ( state == camera_action_state::none){
        reset_stroke = true;//next time we draw a curve, we erase the previous one
    }

    // Update previous click position
    x0 = x1;
    y0 = y1;

}


void camera_control_glfw::update_mouse_click(camera_scene& camera, GLFWwindow* window, int , int , int )
{
    double xpos=0.0, ypos=0.0;
    glfwGetCursorPos(window,&xpos,&ypos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    const float w = static_cast<float>(width);
    const float h = static_cast<float>(height);

    const float x = 2*float(xpos)/float(w)-1;
    const float y = 1-2*float(ypos)/float(h);

    picking_ray(camera, x, y);


}




}
