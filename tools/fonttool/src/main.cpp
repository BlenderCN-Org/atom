#include <iostream>
#include <boost/lexical_cast.hpp>
#include <core/video/image.h>

using std::cout;
using std::endl;
using namespace atom;

inline bool pixel_state(const Image &image, unsigned x, unsigned y)
{
  const PixelR *pixels = reinterpret_cast<const PixelR *>(image.pixels());
  const PixelR *p = &pixels[y * image.width() + x];
  return p->r > 128;
//  return p->r * p->g > 16384;
}

inline float pixel_distance(float  x1, float y1, float x2, float y2)
{
  float dx = x2 - x1;
  float dy = y2 - y1;
  return sqrt(dx * dx + dy * dy);
}

float find_pixel(bool state, const Image &image, float x, float y)
{
  unsigned xmin = x;
  unsigned xmax = x;
  unsigned ymin = y;
  unsigned ymax = y;
  unsigned width = image.width();
  unsigned height = image.height();
  float distance = std::numeric_limits<float>::max();

  bool left, right, top, bottom;
  bool run = true;


  while (run) {
    left   = xmin > 0          && (x - xmin - 1) < distance;
    right  = xmax < width - 1  && (xmax + 1 - x) < distance;
    top    = ymin > 0          && (y - ymin - 1) < distance;
    bottom = ymax < height - 1 && (ymax + 1 - y) < distance;

    if (left) {
      for (unsigned j = ymin; j <= ymax; ++j)
        if (pixel_state(image, xmin - 1, j) == state) {
          distance = std::min(distance, pixel_distance(x, y, xmin - 1 + 0.5, j + 0.5));
        }
    }

    if (right) {
      for (unsigned j = ymin; j <= ymax; ++j)
        if (pixel_state(image, xmax + 1, j) == state) {
          distance = std::min(distance, pixel_distance(x, y, xmax + 1 + 0.5, j + 0.5));
        }
    }

    if (top) {
      for (unsigned j = xmin; j <= xmax; ++j)
        if (pixel_state(image, j, ymin - 1) == state) {
          distance = std::min(distance, pixel_distance(x, y, j + 0.5, ymin - 1 + 0.5));
        }
    }

    if (bottom) {
      for (unsigned j = xmin; j <= xmax; ++j)
        if (pixel_state(image, j, ymax + 1) == state) {
          distance = std::min(distance, pixel_distance(x, y, j + 0.5, ymax - 1 + 0.5));
        }
    }

    if (top && left) {
      if (pixel_state(image, xmin - 1, ymin - 1) == state) {
        distance = std::min(distance, pixel_distance(x, y, xmin - 1 + 0.5, ymin - 1 + 0.5));
      }
    }

    if (top && right) {
      if (pixel_state(image, xmax + 1, ymin - 1) == state) {
        distance = std::min(distance, pixel_distance(x, y, xmax + 1 + 0.5, ymin - 1 + 0.5));
      }
    }

    if (bottom && left) {
      if (pixel_state(image, xmin - 1, ymax + 1) == state) {
        distance = std::min(distance, pixel_distance(x, y, xmin - 1 + 0.5, ymax + 1 + 0.5));
      }
    }

    if (bottom && right) {
      if (pixel_state(image, xmax + 1, ymax + 1) == state) {
        distance = std::min(distance, pixel_distance(x, y, xmax + 1 + 0.5, ymax + 1 + 0.5));
      }
    }

    if (left)   --xmin;
    if (right)  ++xmax;
    if (top)    --ymin;
    if (bottom) ++ymax;

    run = left || right || bottom || top;
  }

  return distance;
}

uptr<Image> generate_distance_image(
  const Image &input,
  const Image &state,
  float spread)
{
  assert(input.format() == PixelFormat::R);

  unsigned src_width = input.width();
  unsigned dst_width = state.width();
  unsigned dst_height = state.height();
  float scale = (float)src_width / state.width();
  uptr<Image> output(new Image(PixelFormat::R, dst_width, dst_height));

  PixelR *dst = reinterpret_cast<PixelR *>(output->pixels());
  const PixelR *state_pixels = reinterpret_cast<const PixelR *>(state.pixels());

//  float spread_factor = 128.0f / spread;

  #pragma omp parallel for schedule(dynamic)
  for (unsigned y = 0; y < dst_height; ++y) {
    info("Row %i", y);
    for (unsigned x = 0; x < dst_width; ++x) {
      PixelR &p = dst[y * dst_width + x];
      bool current_state = pixel_state(state, x, y);
      float distance = find_pixel(!current_state, input, x * scale + scale / 2, y * scale + scale / 2);

      if (current_state) {
        p.r = 127 + std::min(128.0f, distance * spread);
      } else {
        p.r = 127 - std::min(127.0f, distance * spread);
      }

      // nakreslis len stav in/out
//      p.r = 255 * current_state;
    }
  }

  return output;
}

int main(int argc, char *argv[])
{
  if (argc != 5) {
    error("Invalid arguments, filename and ??? is required");
    error("Usage: mmfont bigimage.png smallimage.png output.png spread");
    return EXIT_FAILURE;
  }

  const char *input_filename = argv[1];
  const char *state_filename = argv[2];
  const char *output_filename = argv[3];
  float spread = boost::lexical_cast<float>(argv[4]);

  uptr<Image> src = Image::create_from_file(input_filename);
  uptr<Image> state = Image::create_from_file(state_filename);

  if (src == nullptr) {
    error("Can't open source image \"%s\"", input_filename);
    return EXIT_FAILURE;
  }

  if (src->width() == 0 || src->height() == 0) {
    error("Source image has an invalid size %ix%i", src->width(), src->height());
    return EXIT_FAILURE;
  }

  if (src->format() != PixelFormat::R) {
    error("Image \"%s\" has invalid format (supported are 1channel images only)", input_filename);
    return EXIT_FAILURE;
  }

  if (state == nullptr) {
    error("Can't open source image \"%s\"", state_filename);
    return EXIT_FAILURE;
  }

  if (state->width() == 0 || state->height() == 0) {
    error("State image has an invalid size %ix%i", state->width(), state->height());
    return EXIT_FAILURE;
  }

  if (state->format() != PixelFormat::R) {
    error("State image has invalid format (1channel images only)");
    return EXIT_FAILURE;
  }

  info("Output size %ix%i", state->width(), state->height());

  uptr<Image> dst = generate_distance_image(*src, *state, spread);
  dst->save_to_png(output_filename);

  return EXIT_SUCCESS;
}
