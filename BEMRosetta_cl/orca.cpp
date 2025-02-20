// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 - 2023, the BEMRosetta author and contributors
#include "BEMRosetta.h"
#include "BEMRosetta_int.h"
#include "FastOut.h"
#ifdef PLATFORM_WIN32
#include "orca.h"

int Orca::deltaLogSimulation = 10;
UVector<int> Orca::objTypes;
UVector<String> Orca::objNames;
UVector<HINSTANCE> Orca::objHandles;
int Orca::actualBlade;
UVector<int> Orca::varIDs, Orca::varBlades;
UVector<String> Orca::varNames, Orca::varFullNames, Orca::varUnits;

VectorMap<int, String> Orca::state = {
	{msReset, "Reset"},
	{msCalculatingStatics, "Calculating Statics"},
	{msInStaticState, "In Static State"},
	{msRunningSimulation, "Running Simulation"},
	{msSimulationStopped, "Simulation finished OK"},
	{msSimulationStoppedUnstable, "Simulation Stopped Unstable"}
};

VectorMap<int, String> Orca::objectTypes = {
	{otNull, "Null"},
	{otGeneral, "General"},
	{otEnvironment, "Environment"},
	{otVessel, "Vessel"},
	{otLine, "Line"},
	{ot6DBuoy, "6DBuoy"},
	{ot3DBuoy, "3DBuoy"},
	{otWinch, "Winch"},
	{otLink , "Link"},
	{otShape , "Shape"},
	{otConstraint, "Constraint"},
	{otTurbine, "Turbine"},
	{otDragChain, "DragChain"},
	{otLineType, "LineType"},
	{otClumpType, "ClumpType"},
	{otWingType, "WingType"},
	{otVesselType, "VesselType"},
	{otDragChainType, "DragChainType"},
	{otFlexJointType, "FlexJointType"},
	{otStiffenerType, "StiffenerType"},
	{otFlexJoint, "FlexJoint"},
	{otAttachedBuoy, "AttachedBuoy"},
	{otFrictionCoefficients, "FrictionCoefficients"},
	{otSolidFrictionCoefficients, "FrictionCoefficients"},
	{otRayleighDampingCoefficients, "RayleighDampingCoefficients"},
	{otWakeModel, "WakeModel"},
	{otPyModel, "PyModel"},
	{otLineContact, "LineContact"},
	{otCodeChecks, "CodeChecks"},
	{otShear7Data, "Shear7Data"},
	{otVIVAData, "VIVAData"},
	{otSupportType, "SupportType"},
	{otMorisonElementType, "MorisonElementType"},
	{otExpansionTable, "ExpansionTable"},
	{otBrowserGroup, "BrowserGroup"},
	{otMultiBodyGroup, "MultiBodyGroup"},
    {otMultipleObjects, "MultipleObjects"},
    {otDragCoefficient, "DragCoefficient"},
    {otAxialStiffness, "AxialStiffness"},
    {otBendingStiffness, "BendingStiffness"},
    {otBendingConnectionStiffness, "BendingConnectionStiffness"},
    {otWingOrientation, "WingOrientation"},
    {otKinematicViscosity, "KinematicViscosity"},
    {otFluidTemperature, "FluidTemperature"},
    {otCurrentSpeed, "CurrentSpeed"},
    {otCurrentDirection, "CurrentDirection"},
    {otExternalFunction, "ExternalFunction"},
    {otHorizontalVariationFactor, "HorizontalVariationFactor"},
    {otLoadForce, "LoadForce"},
    {otLoadMoment, "LoadMoment"},
    {otExpansionFactor, "ExpansionFactor"},
    {otPayoutRate, "PayoutRate"},
    {otWinchPayoutRate, "PayoutRate"},
    {otWinchTension, "WinchTension"},
    {otVerticalVariationFactor, "VerticalVariationFactor"},
    {otTorsionalStiffness, "TorsionalStiffness"},
    {otMinimumBendRadius, "MinimumBendRadius"},
    {otLiftCoefficient, "LiftCoefficient"},
    {otLiftCloseToSeabed, "LiftCloseToSeabed"},
    {otDragCloseToSeabed, "DragCloseToSeabed"},
    {otDragAmplificationFactor, "DragAmplificationFactor"},
    {otLineTypeDiameter, "LineTypeDiameter"},
    {otStressStrainRelationship, "StressStrainRelationship"},
    {otCoatingOrLining, "CoatingOrLining"},
    {otContentsFlowVelocity, "ContentsFlowVelocity"},
    {otAddedMassRateOfChangeCloseToSurface, "AddedMassRateOfChangeCloseToSurface"},
    {otAddedMassCloseToSurface, "AddedMassCloseToSurface"},
    {otContactStiffness, "ContactStiffness"},
    {otSupportsStiffness, "SupportsStiffness"},
    {otConstraintTranslationalStiffness, "ConstraintTranslationalStiffness"},
    {otConstraintRotationalStiffness, "ConstraintRotationalStiffness"},
    {otConstraintTranslationalDamping, "ConstraintTranslationalDamping"},
    {otConstraintRotationalDamping, "ConstraintRotationalDamping"},
    {otAddedMassCloseToSeabed, "AddedMassCloseToSeabed"},
    {otSeabedTangentialResistance, "SeabedTangentialResistance"}
};

#endif