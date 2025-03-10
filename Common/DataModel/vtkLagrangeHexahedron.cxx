// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkLagrangeHexahedron.h"

#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkHexahedron.h"
#include "vtkIdList.h"
#include "vtkLagrangeCurve.h"
#include "vtkLagrangeInterpolation.h"
#include "vtkLagrangeQuadrilateral.h"
#include "vtkLine.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkTriangle.h"
#include "vtkVector.h"
#include "vtkVectorOperators.h"

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkLagrangeHexahedron);

vtkLagrangeHexahedron::vtkLagrangeHexahedron() = default;

vtkLagrangeHexahedron::~vtkLagrangeHexahedron() = default;

void vtkLagrangeHexahedron::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

vtkCell* vtkLagrangeHexahedron::GetEdge(int edgeId)
{
  vtkLagrangeCurve* result = EdgeCell;
  const auto set_number_of_ids_and_points = [&](const vtkIdType& npts) -> void {
    result->Points->SetNumberOfPoints(npts);
    result->PointIds->SetNumberOfIds(npts);
  };
  const auto set_ids_and_points = [&](const vtkIdType& face_id, const vtkIdType& vol_id) -> void {
    result->Points->SetPoint(face_id, this->Points->GetPoint(vol_id));
    result->PointIds->SetId(face_id, this->PointIds->GetId(vol_id));
  };

  this->SetEdgeIdsAndPoints(edgeId, set_number_of_ids_and_points, set_ids_and_points);
  return result;
}

vtkCell* vtkLagrangeHexahedron::GetFace(int faceId)
{
  vtkLagrangeQuadrilateral* result = FaceCell;
  int faceOrder[2];

  const auto set_number_of_ids_and_points = [&](const vtkIdType& npts) -> void {
    result->Points->SetNumberOfPoints(npts);
    result->PointIds->SetNumberOfIds(npts);
  };
  const auto set_ids_and_points = [&](const vtkIdType& face_id, const vtkIdType& vol_id) -> void {
    result->Points->SetPoint(face_id, this->Points->GetPoint(vol_id));
    result->PointIds->SetId(face_id, this->PointIds->GetId(vol_id));
  };

  vtkHigherOrderHexahedron::SetFaceIdsAndPoints(
    faceId, this->Order, set_number_of_ids_and_points, set_ids_and_points, faceOrder);
  result->SetOrder(faceOrder[0], faceOrder[1]);
  return result;
}

/**\brief Populate the linear hex returned by GetApprox() with point-data from one voxel-like
 * intervals of this cell.
 *
 * Ensure that you have called GetOrder() before calling this method
 * so that this->Order is up to date. This method does no checking
 * before using it to map connectivity-array offsets.
 */
vtkHexahedron* vtkLagrangeHexahedron::GetApproximateHex(
  int subId, vtkDataArray* scalarsIn, vtkDataArray* scalarsOut)
{
  vtkHexahedron* approx = this->GetApprox();
  bool doScalars = (scalarsIn && scalarsOut);
  if (doScalars)
  {
    scalarsOut->SetNumberOfTuples(8);
  }
  int i, j, k;
  if (!this->SubCellCoordinatesFromId(i, j, k, subId))
  {
    vtkErrorMacro("Invalid subId " << subId);
    return nullptr;
  }
  // Get the point coordinates (and optionally scalars) for each of the 8 corners
  // in the approximating hexahedron spanned by (i, i+1) x (j, j+1) x (k, k+1):
  for (vtkIdType ic = 0; ic < 8; ++ic)
  {
    const vtkIdType corner = this->PointIndexFromIJK(
      i + ((((ic + 1) / 2) % 2) ? 1 : 0), j + (((ic / 2) % 2) ? 1 : 0), k + ((ic / 4) ? 1 : 0));
    vtkVector3d cp;
    this->Points->GetPoint(corner, cp.GetData());
    approx->Points->SetPoint(ic, cp.GetData());
    approx->PointIds->SetId(ic, doScalars ? corner : this->PointIds->GetId(corner));
    if (doScalars)
    {
      scalarsOut->SetTuple(ic, scalarsIn->GetTuple(corner));
    }
  }
  return approx;
}

void vtkLagrangeHexahedron::InterpolateFunctions(const double pcoords[3], double* weights)
{
  vtkLagrangeInterpolation::Tensor3ShapeFunctions(this->GetOrder(), pcoords, weights);
}

void vtkLagrangeHexahedron::InterpolateDerivs(const double pcoords[3], double* derivs)
{
  vtkLagrangeInterpolation::Tensor3ShapeDerivatives(this->GetOrder(), pcoords, derivs);
}
vtkHigherOrderCurve* vtkLagrangeHexahedron::GetEdgeCell()
{
  return EdgeCell;
}
vtkHigherOrderQuadrilateral* vtkLagrangeHexahedron::GetFaceCell()
{
  return FaceCell;
}
vtkHigherOrderInterpolation* vtkLagrangeHexahedron::GetInterpolation()
{
  return Interp;
};
VTK_ABI_NAMESPACE_END
