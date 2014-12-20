#pragma once

#include <fstream>
#include <rapidjson/document.h>
#include "foundation.h"
#include "stdvec.h"

namespace atom {
namespace utils {

enum class ReadResult {
  OK,   ///< hodnota nacitana uspesne
  FAIL, ///< chyba pri nacitani hodnoty (napr. citam cislo ale dostanem string, ...)
  MISS  ///< dane pole neexistuje, preto ho nieje mozne nacitat
};

/**
 * Nacitaj hodnotu (membra) z json objektu (tato funkcia dekoduje len zakladne typy).
 */
ReadResult read_basic_property_from_json(const rapidjson::Value &obj, const MetaField &p, void *data);

/**
 * Nacitaj hodnotu z json reprezentacie (podporovane su zakladne aj rozsirene typy ako shadery, ...).
 */
ReadResult read_property_from_json(const rapidjson::Value &obj, const MetaField &p, void *data, ResourceService &rs);

bool write_basic_property_to_json(rapidjson::Document &doc, rapidjson::Value &obj,
  const MetaField &field, void *data);

//
// Json utility
//

/**
 * Vrat hodnotu cisla z json uzlu. Konkretne specializacie volaju GetDouble, GetInt, ... podla typu T.
 */
template<typename T>
T get_number_from_json(const rapidjson::Value &value);

template<>
int get_number_from_json(const rapidjson::Value &value);

template<>
u32 get_number_from_json(const rapidjson::Value &value);

template<>
float get_number_from_json(const rapidjson::Value &value);

template<typename T>
bool read_array(const rapidjson::Value &node, std::vector<T> &value);

bool read_vec2f(const rapidjson::Value &node, Vec2f &v);

bool read_vec3f(const rapidjson::Value &node, Vec3f &v);

bool read_vec4f(const rapidjson::Value &node, Vec4f &v);

class JsonInputStream {
public:
  typedef char Ch;

  explicit JsonInputStream(std::ifstream &input);

  char Peek();
  char Take();

  std::size_t Tell();

  void Put(char c);

  char* PutBegin();
  std::size_t PutEnd(char *);

private:
  char transform_eof(int ch);

private:
  std::ifstream *my_input;
};

class JsonOutputStream {
public:
  explicit JsonOutputStream(std::ofstream &output);

  std::size_t Tell();

  void Put(char c);

private:
  std::ofstream *my_output;
};

}
}
