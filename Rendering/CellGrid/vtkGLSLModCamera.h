// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @class   vtkGLSLModCamera
 * @brief   Implement light kit support in the OpenGL renderer for vtkCellGrid.
 */

#ifndef vtkGLSLModCamera_h
#define vtkGLSLModCamera_h

#include "vtkGLSLModifierBase.h"

#include "vtkMatrix3x3.h"               // for ivar
#include "vtkMatrix4x4.h"               // for ivar
#include "vtkRenderingCellGridModule.h" // for export macro
#include "vtkWeakPointer.h"             // for ivar

VTK_ABI_NAMESPACE_BEGIN
class vtkActor;
class vtkInformationObjectBaseKey;

class VTKRENDERINGCELLGRID_EXPORT vtkGLSLModCamera : public vtkGLSLModifierBase
{
public:
  static vtkGLSLModCamera* New();
  vtkTypeMacro(vtkGLSLModCamera, vtkGLSLModifierBase);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  // vtkGLSLModifierBase virtuals:
  bool ReplaceShaderValues(vtkOpenGLRenderer* vtkNotUsed(renderer),
    std::string& vtkNotUsed(vertexShader), std::string& vtkNotUsed(geometryShader),
    std::string& vtkNotUsed(fragmentShader), vtkAbstractMapper* vtkNotUsed(mapper),
    vtkActor* vtkNotUsed(actor)) override
  {
    // nothing to replace.
    return true;
  }
  bool SetShaderParameters(vtkOpenGLRenderer* renderer, vtkShaderProgram* program,
    vtkAbstractMapper* mapper, vtkActor* actor, vtkOpenGLVertexArrayObject* VAO = nullptr) override;

  bool IsUpToDate(vtkOpenGLRenderer* vtkNotUsed(renderer), vtkAbstractMapper* vtkNotUsed(mapper),
    vtkActor* vtkNotUsed(actor)) override
  {
    // no replacements were done. shader is always up-to-date, as far as this mod is concerned.
    return true;
  }

protected:
  vtkGLSLModCamera();
  ~vtkGLSLModCamera() override;

  vtkNew<vtkMatrix3x3> TempMatrix3;
  vtkNew<vtkMatrix4x4> TempMatrix4;

private:
  vtkGLSLModCamera(const vtkGLSLModCamera&) = delete;
  void operator=(const vtkGLSLModCamera&) = delete;
};

VTK_ABI_NAMESPACE_END
#endif
