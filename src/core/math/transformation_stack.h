#pragma once

#include "core/math/transformations.h"
#include "core/math/mat_array.h"

namespace atom {

/// #todo comment
//inline Transformations calculate_transformations(
//  const Mat4f &model,
//  const Mat4f &view,
//  const Mat4f &projection)
//{
//  Transformations t;
//  t.model = model;
//  t.model_view = view * model;
//  t.model_view_projection = projection * t.model_view;
//  return t;
//}

/**
 * Trieda obalujuca logiku okolo zobrazovacich transformacnych matic.
 */
class TransformationStack {
public:
  // public methods
  /**
   * Vychodzi konstruktor, umoznuje aj inicializaciu.
   * Inicializa vlozi do jednotlivych zasobnikov po jednej
   * identickej matici.
   *
   * @param initialize true ak chceme automaticku inicializaciu
   */
  explicit TransformationStack(
    bool initialize = true)
  {
    if (initialize) {
      my_model_stack.push_back(Mat4f::identity());
      my_view_stack.push_back(Mat4f::identity());
      my_projection_stack.push_back(Mat4f::identity());
    }
  }

  /**
   * Vloz maticu na vrchol MODEL zasobnika
   */
  void push_model_matrix(
    const Mat4f &model,
    bool combine = false)
  { my_model_stack.push_back(combine ? model * top_model_matrix() : model); }

  /**
   * Vyber Maticu z vrcholu MODEL zasobnika.
   */
  void pop_model_matrix()
  {
    assert(!my_model_stack.empty());
    my_model_stack.pop_back();
  }

  /**
   * Vloz maticu na vrchol VIEW zasobnika
   */
  void push_view_matrix(
    const Mat4f &view,
    bool combine = false)
  { my_view_stack.push_back(combine ? view * top_view_matrix() : view); }

  /**
   * Vyber Maticu z vrcholu VIEW zasobnika.
   */
  void pop_view_matrix()
  {
    assert(!my_view_stack.empty());
    my_view_stack.pop_back();
  }

  /**
   * Vloz maticu na vrchol PROJECTION zasobnika
   */
  void push_projection_matrix(
    const Mat4f &projection,
    bool combine = false)
  { my_projection_stack.push_back(combine ? projection * top_projection_matrix() : projection); }

  /**
   * Vyber Maticu z vrcholu PROJECTION zasobnika.
   */
  void pop_projection_matrix()
  {
    assert(!my_projection_stack.empty());
    my_projection_stack.pop_back();
  }

  /**
   * Utility metoda pre vlozenie vsetkych troch matic naraz.
   *
   * Zatial nepodporuje kombinovanie matic v zasobniku.
   */
  void push_all(
    const Mat4f &model,
    const Mat4f &view,
    const Mat4f &projection)
  {
    push_model_matrix(model, false);
    push_view_matrix(view, false);
    push_projection_matrix(projection, false);
  }

  /**
   * Utility metoda pre vybratie vsetkych troch matic naraz.
   */
  void pop_all()
  {
    pop_model_matrix();
    pop_view_matrix();
    pop_projection_matrix();
  }

  /**
   * Ziskaj konstantnu (referenciu na) maticu na vrchole MODEL zasobnika.
   */
  const Mat4f& top_model_matrix() const
  {
    assert(!my_model_stack.empty());
    return my_model_stack.back();
  }

  /**
   * Ziskaj konstantnu (referenciu na) maticu na vrchole VIEW zasobnika.
   */
  const Mat4f& top_view_matrix() const
  {
    assert(!my_view_stack.empty());
    return my_view_stack.back();
  }

  /**
   * Ziskaj konstantnu (referenciu na) maricu na vrchole PROJECTION zasobnika.
   */
  const Mat4f& top_projection_matrix() const
  {
    assert(!my_projection_stack.empty());
    return my_projection_stack.back();
  }

  Mat4f get_model_view() const
  {
    assert(!my_model_stack.empty());
    assert(!my_view_stack.empty());

    return top_view_matrix() * top_model_matrix();
  }

  /**
   * Vypocitaj transformacnu maticu obsahujucu MODEL VIEW aj PROJECTION transformacie.
   *
   * @todo dorobit cache/predpocitavanie pretoze tato metoda sa moze volat viac krat???
   *       v pripade optimalizacie by mala vracat const Matrix& a nemala by byt const,
   *       pretoze bude musiet zapisovat do seba
   */
  Mat4f get_model_view_projection() const
  {
    assert(!my_model_stack.empty());
    assert(!my_view_stack.empty());
    assert(!my_projection_stack.empty());

    return (top_projection_matrix() * top_view_matrix()) * top_model_matrix();
  }

  Transformations get_transformations() const
  { return Transformations(top_model_matrix(), top_view_matrix(), top_projection_matrix()); }

private:
  // private members
  Mat4fArray my_model_stack; ///< rotacia/posunutie objektu
  Mat4fArray my_view_stack; ///< kamera/pohlad
  Mat4fArray my_projection_stack; ///< perspektivna/ortograficka projekcia
};

}
