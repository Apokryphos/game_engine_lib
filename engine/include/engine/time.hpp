#pragma once

namespace engine
{
//  Gets elapsed seconds since last frame
double get_elapsed_seconds();

//  Gets elapsed seconds since program start
double get_total_elapsed_seconds();

void update_time();
}
