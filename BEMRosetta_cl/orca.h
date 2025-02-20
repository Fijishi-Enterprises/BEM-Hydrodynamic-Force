// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 - 2023, the BEMRosetta author and contributors
#ifdef PLATFORM_WIN32
;
#pragma pack(push, 1)

// Object type constants 
#define otNull 0
#define otGeneral 1
#define otEnvironment 3
#define otVessel 5
#define otLine 6
#define ot6DBuoy 7
#define ot3DBuoy 8
#define otWinch 9
#define otLink 10
#define otShape 11
#define otConstraint 12
#define otTurbine 13
#define otDragChain 14
#define otLineType 15
#define otClumpType 16
#define otWingType 17
#define otVesselType 18
#define otDragChainType 19
#define otFlexJointType 20
#define otStiffenerType 21
#define otFlexJoint 41
#define otAttachedBuoy 43
#define otFrictionCoefficients 47
#define otSolidFrictionCoefficients otFrictionCoefficients
#define otRayleighDampingCoefficients 48
#define otWakeModel 49
#define otPyModel 50
#define otLineContact 51
#define otCodeChecks 52
#define otShear7Data 53
#define otVIVAData 54
#define otSupportType 55
#define otMorisonElementType 57
#define otExpansionTable 58
#define otBrowserGroup 61
#define otMultiBodyGroup 62
#define otMultipleObjects 56
#define otDragCoefficient 1000
#define otAxialStiffness 1001
#define otBendingStiffness 1002
#define otBendingConnectionStiffness 1003
#define otWingOrientation 1004
#define otKinematicViscosity 1005
#define otFluidTemperature 1006
#define otCurrentSpeed 1007
#define otCurrentDirection 1008
#define otExternalFunction 1009
#define otHorizontalVariationFactor 1010
#define otLoadForce 1011
#define otLoadMoment 1012
#define otExpansionFactor 1013
#define otPayoutRate 1014
#define otWinchPayoutRate otPayoutRate 
#define otWinchTension 1015
#define otVerticalVariationFactor 1016
#define otTorsionalStiffness 1017
#define otMinimumBendRadius 1018
#define otLiftCoefficient 1019
#define otLiftCloseToSeabed 1020
#define otDragCloseToSeabed 1021
#define otDragAmplificationFactor 1022
#define otLineTypeDiameter 1023
#define otStressStrainRelationship 1024
#define otCoatingOrLining 1025
#define otContentsFlowVelocity 1026
#define otAddedMassRateOfChangeCloseToSurface 1027
#define otAddedMassCloseToSurface 1028
#define otContactStiffness 1029
#define otSupportsStiffness 1030
#define otConstraintTranslationalStiffness 1031
#define otConstraintRotationalStiffness 1032
#define otConstraintTranslationalDamping 1033
#define otConstraintRotationalDamping 1034
#define otAddedMassCloseToSeabed 1035
#define otSeabedTangentialResistance 1036

// For GetModelState 
#define msReset 0
#define msCalculatingStatics 1
#define msInStaticState 2
#define msRunningSimulation 3
#define msSimulationStopped 4
#define msSimulationStoppedUnstable 5

// For TObjectExtra.LinePoint 
#define ptEndA 0
#define ptEndB 1
#define ptTouchdown 2
#define ptNodeNum 3
#define ptArcLength 4

/* Licence reconnection constants */
#define lrBegin 0
#define lrContinue 1
#define lrEnd 2

typedef wchar_t TObjectName[50];
typedef struct {
    HINSTANCE ObjectHandle;
    int ObjectType;
    TObjectName ObjectName;
} TObjectInfo;

typedef double TVector[3];

typedef struct {
    int Size;
    TVector EnvironmentPos;
    int LinePoint;
    int NodeNum;
    double ArcLength;
    int RadialPos;
    double Theta;
    LPCWSTR WingName;
    LPCWSTR ClearanceLineName;
    int WinchConnectionPoint;
    TVector RigidBodyPos;
    LPCWSTR ExternalResultText;
    LPCWSTR DisturbanceVesselName;
    int SupportIndex;
    LPCWSTR SupportedLineName;
    int BladeIndex;
    int ElementIndex;
    double SeaSurfaceScalingFactor;
} TObjectExtra2;

typedef struct {
	int Size;
	int VarID;
	LPCWSTR lpVarName;
	LPCWSTR lpVarUnits;
	LPCWSTR lpFullName;
	HINSTANCE ObjectHandle;
} TVarInfo;

typedef void (__stdcall *TProgressHandlerProc)(HINSTANCE, LPCWSTR, int *);
typedef void (__stdcall *TSimulationHandlerProc)(HINSTANCE handle, double SimulationTime, double SimulationStart, double SimulationStop, int *);
typedef void (__stdcall *TEnumerateObjectsProc)(HINSTANCE handle, const TObjectInfo*);
typedef void (__stdcall *TEnumerateVarsProc)(const TVarInfo *lpVarInfo);
typedef void (__stdcall *TLicenceNotFoundHandlerProc)(int, BOOL*, INT_PTR*);

typedef struct {
	int Size;
	BOOL EnableAutoSave;
	int AutoSaveIntervalMinutes;
	LPCTSTR AutoSaveFileName;
} TRunSimulationParameters;
typedef struct {
	int PeriodNum;
	int Unused;
	double FromTime;
	double ToTime;
} TPeriod;

#define pnSpecifiedPeriod 	 32001
#define pnLatestWave 		 32002
#define pnWholeSimulation 	 32003
#define pnStaticState 		 32004
#define pnInstantaneousValue 32005

#define rtTimeHistory 0

class Orca {
public:
	~Orca() {
		if (dll.GetHandle() != 0) {
			int status;
			
			if (wave) {
				DestroyDiffraction(wave, &status);
				if (status != 0)
					throwError("DestroyDiffraction");
			}
			
			if (flex) {
				DestroyModel(flex, &status);
				if (status != 0)
					throwError("DestroyModel");
			}
			FinaliseLibrary(&status);
			if (status != 0)
				throwError("FinaliseLibrary");
		}
	}
	
	bool Init(String _dllFile) {
		dllFile = _dllFile;
		if (!dll.Load(dllFile))
			return false;

		CreateModel  = DLLGetFunction(dll, void, C_CreateModel, (HINSTANCE *handle, HWND hCaller, int *status));
		DestroyModel = DLLGetFunction(dll, void, C_DestroyModel,(HINSTANCE handle, int *status));
		LoadData     = DLLGetFunction(dll, void, C_LoadDataW,   (HINSTANCE handle, LPCWSTR wcs, int *status));
		SaveData     = DLLGetFunction(dll, void, C_SaveDataW,   (HINSTANCE handle, LPCWSTR wcs, int *status));
		
		CreateDiffraction      = DLLGetFunction(dll, void, C_CreateDiffraction, 	  (HINSTANCE *handle, int *status));
		DestroyDiffraction     = DLLGetFunction(dll, void, C_DestroyDiffraction,	  (HINSTANCE handle, int *status));
		LoadDiffractionData    = DLLGetFunction(dll, void, C_LoadDiffractionDataW,    (HINSTANCE handle, LPCWSTR wcs, int *status));
		SaveDiffractionData    = DLLGetFunction(dll, void, C_SaveDiffractionDataW,    (HINSTANCE handle, LPCWSTR wcs, int *status));
		CalculateDiffraction   = DLLGetFunction(dll, void, C_CalculateDiffractionW,	  (HINSTANCE handle, TProgressHandlerProc proc, int *status));
		SaveDiffractionResults = DLLGetFunction(dll, void, C_SaveDiffractionResultsW, (HINSTANCE handle, LPCWSTR wcs, int *status));
		CalculateStatics	   = DLLGetFunction(dll, void, C_CalculateStaticsW, 	  (HINSTANCE handle, TProgressHandlerProc proc, int *status));
		RunSimulation		   = DLLGetFunction(dll, void, C_RunSimulation2W, 		  (HINSTANCE handle, TSimulationHandlerProc proc, const TRunSimulationParameters *lpRunSimulationParameters, int *status));
		LoadSimulation		   = DLLGetFunction(dll, void, C_LoadSimulationW,		  (HINSTANCE handle, LPCWSTR wcs, int *status));
		SaveSimulation		   = DLLGetFunction(dll, void, C_SaveSimulationW,		  (HINSTANCE handle, LPCWSTR wcs, int *status));
		GetTimeHistory2		   = DLLGetFunction(dll, void, C_GetTimeHistory2W,		  (HINSTANCE handle, void *nil, const TPeriod *period, int varID, double *lpValues, int *status));
		GetVarID			   = DLLGetFunction(dll, void, C_GetVarIDW,				  (HINSTANCE handle, LPCWSTR wcs, int *lpVarID, int *status));
		GetNumOfSamples		   = DLLGetFunction(dll, int,  C_GetNumOfSamples, 		  (HINSTANCE handle, const TPeriod *period, int *status));
		EnumerateObjects	   = DLLGetFunction(dll, void, C_EnumerateObjectsW, 	  (HINSTANCE handle, TEnumerateObjectsProc proc, int *lpNumOfObjects, int *status));
		EnumerateVars2		   = DLLGetFunction(dll, void, C_EnumerateVars2W, 		  (HINSTANCE handle, const TObjectExtra2 *objectextra, int ResultType, 
																							TEnumerateVarsProc EnumerateVarsProc, int *lpNumberOfVars, int *status));
		ObjectCalled		   = DLLGetFunction(dll, void, C_ObjectCalledW,			  (HINSTANCE handle, LPCWSTR lpObjectName, TObjectInfo *lpObjectInfo, int *status));
		CGetModelState		   = DLLGetFunction(dll, void, C_GetModelState,		  	  (HINSTANCE handle, int *lpModelState, int *status));
		
		SetModelThreadCount = DLLGetFunction(dll, void, C_SetModelThreadCount, (HINSTANCE handle, int threadCount, int *status));
		GetModelThreadCount = DLLGetFunction(dll, int, C_GetModelThreadCount,  (HINSTANCE handle, int *status));
		
		RegisterLicenceNotFoundHandler = DLLGetFunction(dll, void, C_RegisterLicenceNotFoundHandler, 	  (TLicenceNotFoundHandlerProc Handler, int *lpStatus));
		
		GetLastErrorString = DLLGetFunction(dll, int,  C_GetLastErrorStringW, (LPCWSTR wcs));
		FinaliseLibrary    = DLLGetFunction(dll, void, C_FinaliseLibrary,     (int *status));
		
		return true;
	}
	
	bool FindInit() {
		UArray<SoftwareDetails> orcadata = GetSoftwareDetails("*OrcaFlex*");	// Get installed versions
		if (orcadata.IsEmpty())
			return false;
		
		int iversion = 0;
		UVector<int> version = orcadata[0].GetVersion();
		for (int i = 1; i < orcadata.size(); ++i) {							// Get the newest version from the installed
			UVector<int> each = orcadata[i].GetVersion();
			if (SoftwareDetails::IsHigherVersion(each, version)) {			// Version are numbers separated by .
				iversion = i;
				version = pick(each);
			}
		}
		String arch;
	#ifdef CPU_64
		arch = "Win64";
	#else
		arch = "Win32";
	#endif
		String path = AppendFileNameX(orcadata[iversion].path, "OrcFxAPI", arch, "OrcFxAPI.dll");	// Assembles the path
		
		return Init(path);
	}
	
	bool IsLoaded()	{return dll;}

	static void __stdcall DiffractionHandlerProc(HINSTANCE handle, LPCWSTR lpProgress, BOOL *lpCancel) {
		static int lastPerc = -1;
		String msg = WideToString(lpProgress);
		int perc = -1;
		int idp = msg.Find("%");
		if (idp >= 0) {
			int idbp = msg.ReverseFind(" ", idp);
			if (idbp >= 0) 
				perc = ScanInt(msg.Mid(idbp+1, idp-idbp));
		}
		if (lastPerc == perc) 
			return;
		
		lastPerc = perc;
		
		Time et;
		if (perc == 0)
			et = Null;
		else {
			int64 sec = GetSysTime() - startCalc - noLicenseTime;
			int64 estsec = int64(100*sec/double(perc));
			et = GetSysTime() + estsec;
		}
		*lpCancel = WhenWave(msg, perc, et);
	}

	static void __stdcall StaticsHandlerProc(HINSTANCE handle, LPCWSTR lpProgress, BOOL *lpCancel) {
		String msg = WideToString(lpProgress);
		*lpCancel = WhenPrint(msg);
	}
	
	static void __stdcall LicenceNotFoundHandler(int action, BOOL *lpAttemptReconnection, INT_PTR *lpData) {
		switch (action) {
		case lrBegin:
			beginNoLicense = GetSysTime();
			Sleep(60*1000); 
			*lpAttemptReconnection = TRUE;
			WhenPrint("License lost. Attemping reconnection in a minute");
			*lpData = 1;
			return;
		case lrContinue:
			*lpAttemptReconnection = TRUE;	//*lpData < 10; 
			if (*lpAttemptReconnection) {
				Sleep(60*1000);
				WhenPrint(Format("License lost for %d min. Attemping reconnection in a minute", *lpData));
				(*lpData)++; 
			}
			return;
		case lrEnd:
			noLicenseTime += (GetSysTime() - beginNoLicense);
		}
	}
	
	static int deltaLogSimulation;
	
	static void __stdcall SimulationHandlerProc(HINSTANCE handle, double simulationTime, double simulationStart, double simulationStop, BOOL *lpCancel) {
		Time tm = GetSysTime();
		if (IsNull(startCalc))		// Time starts here. Statics calculation delay is discarded
			startCalc = tm;
		else if (tm - lastLog < deltaLogSimulation)
			return;
		lastLog = tm;
		double elapsed  = simulationTime - simulationStart,
			   total    = simulationStop - simulationStart;
		int64  elapsedT = tm - startCalc - noLicenseTime,
			   pending  = int64(elapsedT*(total/elapsed - 1));		// elapsedT*total/elapsed - elapsedT
		double ratio = 0;
		if (elapsed == 0)
			ratio = elapsedT/elapsed;
		*lpCancel = WhenPrint(Format("Elap/Total:%.1f/%.0f ET:%s Clk/Sim:%.1f", elapsed, total,
											   					  SecondsToString(double(pending), 0, false, false, true, false, true), elapsedT/elapsed));
	}

	static VectorMap<int, String> objectTypes;
	static VectorMap<int, String> state;
	
	struct Object {
		int type;
		String typeName;
		String name;
		HINSTANCE handle;
	};
	
	static UVector<int> objTypes;
	static UVector<String> objNames;
	static UVector<HINSTANCE> objHandles;
	
	static void __stdcall EnumerateObjectsProc(HINSTANCE handle, const TObjectInfo *info) {
		objTypes << info->ObjectType;
		WString str(info->ObjectName);
		objNames << str.ToString();
		objHandles << info->ObjectHandle;
	}
	
	static int actualBlade;
	static UVector<int> varIDs, varBlades;
	static UVector<String> varNames, varFullNames, varUnits;

	static void __stdcall EnumerateVarsProc(const TVarInfo *lpVarInfo) {
		int id = Find(varIDs, lpVarInfo->VarID);
		if (id < 0 || (actualBlade > 0 && varBlades[id] != actualBlade)) {
			varIDs << lpVarInfo->VarID;
			varBlades << actualBlade;
			varNames << WideToString(lpVarInfo->lpVarName);
			varFullNames << WideToString(lpVarInfo->lpFullName);
			varUnits << WideToString(lpVarInfo->lpVarUnits);
		}
	}
	
	void LoadFlex(String owryml) {
		if (!dll && !FindInit())
			throw Exc("Orca DLL not loaded");
		
		int status;
		LPCWSTR wcs;
		
		if (flex) {
			DestroyModel(flex, &status);
			if (status != 0)
				throwError("LoadFlex DestroyModel");
		}		
		
		CreateModel(&flex, 0, &status);
		if (status != 0)
			throwError("LoadFlex CreateModel: No license available");
		
		if (!StringToWide(owryml, wcs))
			throwError("LoadFlex StringToWide LoadData");
		LoadData(flex, wcs, &status);
		if (status != 0)
			throwError("LoadFlex LoadData");		
	}
	
	void SaveFlex(String owryml) {
		if (!dll && !FindInit())
			throw Exc("Orca DLL not loaded");
		
		int status;
		LPCWSTR wcs;
					
		if (!StringToWide(owryml, wcs))
			throwError("StringToWide SaveData");
		SaveData(flex, wcs, &status);
		if (status != 0)
			throwError("SaveData");
	}
	
	void RunFlex(String to, bool &saved) {
		saved = false;
		
		if (!dll && !FindInit())
			throw Exc("Orca DLL not loaded");
		if (!flex) 
			throwError("RunFlex");	
		
		int status;
		
		RegisterLicenceNotFoundHandler(LicenceNotFoundHandler, &status);
		if (status != 0)
			throwError("RunFlex RegisterLicenceNotFoundHandler");		
		
		startCalc = lastLog = Null;
		noLicenseTime = 0;
		
		CalculateStatics(flex, StaticsHandlerProc, &status);
		if (status != 0)
			throwError("RunFlex CalculateStatics");	
		
		RunSimulation(flex, SimulationHandlerProc, NULL, &status);
		
		WhenPrint(GetFlexSimState(GetModelState()));
		
		if (!IsNull(to) && !IsNull(startCalc)) {
			SaveFlexSim(to);
			saved = true;
		}
		
		if (status != 0)		// The status of previous RunSimulation()
			throwError("RunFlex RunSimulation");	
	}
	
	void SaveFlexSim(String owryml) {
		if (!dll && !FindInit())
			throw Exc("Orca DLL not loaded");
		
		if (!flex) 
			throwError("SaveFlexSim");	
			
		int status;
		LPCWSTR wcs;
					
		if (!StringToWide(owryml, wcs))
			throwError("SaveFlexSim StringToWide");
		SaveSimulation(flex, wcs, &status);
		if (status != 0)
			throwError("SaveFlexSim SaveSimulation");		
	}

	void LoadFlexSim(String sim) {
		if (!dll && !FindInit())
			throw Exc("Orca DLL not loaded");
		
		int status;
		LPCWSTR wcs;
		
		if (flex) {
			DestroyModel(flex, &status);
			if (status != 0)
				throwError("LoadFlexSim");
		}
			
		CreateModel(&flex, 0, &status);
		if (status != 0)
			throwError("LoadFlexSim CreateModel: No license available");
						
		if (!StringToWide(sim, wcs))
			throwError("LoadSimulation StringToWide");
		LoadSimulation(flex, wcs, &status);
		if (status != 0)
			throwError("LoadFlexSim LoadSimulation");		
	}
	
	String GetFlexSimObjectString(int idType) {
		int id = objectTypes.Find(idType);	
		if (id >= 0)
			return objectTypes[id];
		else
			return "UNKNOWN";
	}

	String GetFlexSimState(int idState) {
		int id = state.Find(idState);	
		if (id >= 0)
			return state[id];
		else
			return "UNKNOWN";
	}
		
	void GetFlexSimObjects() {
		if (!dll && !FindInit())
			throw Exc("Orca DLL not loaded");
		
		if (!flex) 
			throwError("GetFlexSimObjects");	
		
		int status;
		int num;
		
		objTypes.Clear();
		objNames.Clear();
		objHandles.Clear();
		EnumerateObjects(flex, EnumerateObjectsProc, &num, &status);
		if (status != 0)
			throwError("GetFlexSimObjects");	
	};
	
	void GetFlexSimVariables(HINSTANCE objHandle, int objType, UVector<int> &IDs, UVector<String> &names, UVector<String> &fullNames, UVector<String> &units) {
		if (!dll && !FindInit())
			throw Exc("Orca DLL not loaded");
		
		if (!flex) 
			throwError("GetFlexSimVariables");	
		
		int status;
		
		TObjectExtra2 objectextra = {};
		objectextra.Size = sizeof(TObjectExtra2);
		
		int ResultType = rtTimeHistory;
		int lpNumberOfVars = -1;
		
		varIDs.Clear();
		varBlades.Clear();
		varNames.Clear();
		varFullNames.Clear();
		varUnits.Clear();
		actualBlade = -1;
		
		EnumerateVars2(objHandle, &objectextra, ResultType, EnumerateVarsProc, &lpNumberOfVars, &status);
		if (status != 0)
			throwError("GetFlexSimVariables.EnumerateVars2");		
		
		if (objType == otTurbine) {		// Adds the parameters related with each blade
			int from = varNames.size();
			for (int ib = 1; ib <= 3; ++ ib) {
				objectextra.BladeIndex = actualBlade = ib;
				EnumerateVars2(objHandle, &objectextra, ResultType, EnumerateVarsProc, &lpNumberOfVars, &status);
				if (status != 0)
					throwError("GetFlexSimVariables.EnumerateVars2");		
				for (int i = from; i < varNames.size(); ++i) {
					varNames[i] = varNames[i] + Format("|Blade|%d", ib);
					varFullNames[i] = varFullNames[i] + Format("|Blade|%d", ib);
				}
				from = varNames.size();
			}
		}
		IDs = pick(varIDs);
		names = pick(varNames);
		fullNames = pick(varFullNames);
		units = pick(varUnits);
	}

	UVector<String> GetFlexSimVariablesList() {
		GetFlexSimObjects();
		
		UVector<String> ret;
		for (int i = 0; i < objHandles.size(); ++i) {
			if (objTypes[i] < 1000) {
				UVector<int> IDs;
				UVector<String> vnames, fullNames, units;
				GetFlexSimVariables(objHandles[i], objTypes[i], IDs, vnames, fullNames, units);
				for (const String &n : vnames)
					ret << Format("%s|%s", objNames[i], n);
			}
		}
		return ret;
	}

	void GetFlexSimVar(HINSTANCE objHandle, int objType, int varId, const Point3D &centre,
					UVector<String> &linePoint, VectorXd &data) {
		if (!dll && !FindInit())
			throw Exc("Orca DLL not loaded");
		
		if (!flex) 
			throwError("GetFlexSimVar");	
		
		int status;
		
		TPeriod period = {};
		period.PeriodNum = pnWholeSimulation;
		
		int numS = GetNumOfSamples(flex, &period, &status);
		if (status != 0)
			throwError("GetFlexSimVar.GetNumOfSamples");
		
		TObjectExtra2 objectExtra = {};
		
		objectExtra.Size = sizeof(TObjectExtra2);
		objectExtra.DisturbanceVesselName = NULL;
		if (objType == otEnvironment) {
			objectExtra.EnvironmentPos[0] = centre.x;
			objectExtra.EnvironmentPos[1] = centre.y;
			objectExtra.EnvironmentPos[2] = centre.z;
		} else if (objType == otVessel || objType == ot6DBuoy) {
			objectExtra.RigidBodyPos[0] = centre.x;
			objectExtra.RigidBodyPos[1] = centre.y;
			objectExtra.RigidBodyPos[2] = centre.z;
		}
		if (linePoint.size() > 0) {
			if (linePoint[0] == "EndA")
				objectExtra.LinePoint = ptEndA;
			else if (linePoint[0] == "EndB")
				objectExtra.LinePoint = ptEndB;
			else if (linePoint[0] == "Touchdown")
				objectExtra.LinePoint = ptTouchdown;
			else if (linePoint[0] == "Node") {
				objectExtra.LinePoint = ptNodeNum;
				if (linePoint.size() != 2)
					throwError("GetFlexSimVar Node number not found");
				int num = ScanInt(linePoint[1]);
				if (IsNull(num) || num < 0 || num > 10000)
					throwError(Format("GetFlexSimVar Invalid node number %s", linePoint[1]));
				objectExtra.NodeNum = num;
			} else if (linePoint[0] == "ArcLength") {
				objectExtra.LinePoint = ptArcLength;
				if (linePoint.size() != 2)
					throwError("GetFlexSimVar Arc length not found");
				double len = ScanDouble(linePoint[1]);
				if (IsNull(len) || len < 0 || len > 10000)
					throwError(Format("GetFlexSimVar Invalid arc length %s", linePoint[1]));
				objectExtra.ArcLength = len;
			} else if (linePoint[0] == "Blade") {	
				if (linePoint.size() != 2)
					throwError("GetFlexSimVar Blade number not found");
				int num = ScanInt(linePoint[1]);
				if (IsNull(num) || num < 1 || num > 3)
					throwError(Format("GetFlexSimVar Invalid blade number %s", linePoint[1]));
				objectExtra.BladeIndex = num;
			} else
				throwError(Format("GetFlexSimVar Invalid option %s", linePoint[0]));
		}
		
		data.resize(numS);
		GetTimeHistory2(objHandle, &objectExtra, &period, varId, data.data(), &status);
		if (status != 0)
			throwError("GetFlexSimVar.GetTimeHistory2");		
	}
	
	void GetFlexSimVar(String name, const Point3D &centre, String &unit, int &objType, VectorXd &data) {
		UVector<String> sp = Split(name, "|");
		if (sp.size() < 2 || sp.size() > 4)
			throwError(Format("GetFlexSimVar incorrect varName %s", name));
		
		String object = sp[0];
		String var = sp[1];
		sp.Remove(0, 2);
		UVector<String> &linePoint = sp;
		if (linePoint.size() == 2 && linePoint[0] == "Blade")
			var << Format("|Blade|%s", linePoint[1]);
		
		GetFlexSimObjects();
		
		HINSTANCE objHandle = 0;
		for (int i = 0; i < objNames.size(); ++i) {
			if (objNames[i] == object) {
				objHandle = objHandles[i];
				objType = objTypes[i];
				break;
			}
		}
		if (objHandle == 0)
			throwError(Format("GetFlexSimVar object '%s' not found", object));
		
		int varId = -1;
		UVector<int> IDs;
		UVector<String> names, fullNames, units;
		GetFlexSimVariables(objHandle, objType, IDs, names, fullNames, units);
		for (int i = 0; i < names.size(); ++i) {
			if (names[i] == var) {
				varId = IDs[i];
				unit = units[i];
				break;
			}
		}
		if (varId == -1)
			throwError(Format("GetFlexSimVar variable '%s' not found", name));
		
		GetFlexSimVar(objHandle, objType, varId, centre, linePoint, data); 
	}
	
	int GetModelState() {
		if (!dll && !FindInit())
			throw Exc("Orca DLL not loaded");
		
		if (!flex) 
			throwError("GetFlexSimVar");	
		
		int status, state;
		CGetModelState(flex, &state, &status);
		if (status != 0)
			throwError("GetModelState");
		
		return state;
	}
	
	String GetStatusString(int idState) {
		int id = state.Find(idState);	
		if (id >= 0)
			return state[id];
		else
			return "UNKNOWN";
	}
	
	void LoadWave(String owryml) {
		if (!dll && !FindInit())
			throw Exc("Orca DLL not loaded");
			
		if (!wave) {
			int status;	
			CreateDiffraction(&wave, &status);
			if (status != 0)
				throwError("CreateDiffraction: No license available");
		}
		
		int status;
		LPCWSTR wcs;
		
		if (!StringToWide(owryml, wcs))
			throwError("StringToWide LoadDiffractionData");
		LoadDiffractionData(wave, wcs, &status);
		if (status != 0)
			throwError("LoadWave.LoadDiffractionData");	
			
		String owd = ForceExt(owryml, ".owd");
		
		if (!StringToWide(owd, wcs))
			throwError("StringToWide LoadDiffractionData SaveDiffractionData");
		SaveDiffractionData(wave, wcs, &status);
		if (status != 0)
			throwError("LoadWave.SaveDiffractionData");
	}
		
	void RunWave() {
		if (!dll && !FindInit())
			throw Exc("Orca DLL not loaded");
		if (!wave) {
			int status;
			CreateDiffraction(&wave, &status);
			if (status != 0)
				throwError("CreateDiffraction");
		}
		
		int status;
		startCalc = GetSysTime();
		noLicenseTime = 0;
		
		CalculateDiffraction(wave, DiffractionHandlerProc, &status);
		if (status != 0)
			throwError("CalculateDiffraction");	
	}
	
	void SaveWaveResults(String owryml) {
		if (!dll && !FindInit())
			throw Exc("Orca DLL not loaded");
		if (!wave) {
			int status;
			CreateDiffraction(&wave, &status);
			if (status != 0)
				throwError("CreateDiffraction");
		}
		
		int status;
		LPCWSTR wcs;
		
		String owr = ForceExt(owryml, ".owr");
		
		if (!StringToWide(owr, wcs))
			throwError("StringToWide SaveData");
		SaveDiffractionResults(wave, wcs, &status);
		if (status != 0)
			throwError("SaveDiffractionResults");		
		
		if (GetFileExt(owryml) == ".yml") {
			HINSTANCE temp;
			
			CreateModel(&temp, 0, &status);
			if (status != 0)
				throwError("CreateModel");
			
			if (!StringToWide(owr, wcs))
				throwError("StringToWide LoadData");
			LoadData(temp, wcs, &status);
			if (status != 0)
				throwError("LoadData");	
			
			if (!StringToWide(owryml, wcs))
				throwError("StringToWide SaveData");
			SaveData(temp, wcs, &status);
			if (status != 0)
				throwError("SaveData");
			
			DestroyModel(temp, &status);
			if (status != 0)
				throwError("DestroyModel");
		}
	}
	
	void SetThreadCount(int nth) {
		if (!dll && !FindInit())
			throw Exc("Orca DLL not loaded");
		if (!wave) {
			int status;
			CreateDiffraction(&wave, &status);
			if (status != 0)
				throwError("CreateDiffraction");
		}
		
		int status;

		SetModelThreadCount(wave, nth, &status);
		if (status != 0)
			throwError("SetThreadCount");		
	}

	int GetThreadCount() {
		if (!dll && !FindInit())
			throw Exc("Orca DLL not loaded");
		if (!wave) {
			int status;
			CreateDiffraction(&wave, &status);
			if (status != 0)
				throwError("CreateDiffraction");
		}
		
		int status;

		int nth = GetModelThreadCount(wave, &status);
		if (status != 0)
			throwError("GetThreadCount");	
			
		return nth;	
	}
	
	void SaveCsv(String file, const UVector<String> &_parameters, const UArray<Point3D> &_centres, String sep) {
		if (_parameters.IsEmpty())
			return;
		if (sep.IsEmpty())
			sep = ";";	
		
		UVector<String> parameters = clone(_parameters);
		UArray<Point3D> centres = clone(_centres);
		if (parameters[0] != "General|Time") {
			parameters.Insert(0, "General|Time");
			centres.Insert(0, Point3D(0, 0, 0));
		}
		
		UVector<String> units(parameters.size());
		UArray<VectorXd> datas(parameters.size());
		for (int i = 0; i < parameters.size(); ++i) {
			String unit;
			int objType;
			VectorXd data;
			GetFlexSimVar(parameters[i], centres[i], unit, objType, data);	
			units[i] = unit;
			datas[i] = pick(data);
			if (objType ==  otEnvironment || objType == otVessel || objType == ot6DBuoy)
				parameters[i] << Format("(%s,%s,%s)", 
						FDS(centres[i].x, 5), FDS(centres[i].y, 5), FDS(centres[i].z, 5)); 
		}
		
		FileOut out(file);
		if (!out.IsOpen())
			throw Exc(Format(t_("Impossible to open '%s'"), file));
		
		for (int i = 0; i < parameters.size(); ++i) {
			if (i > 0)
				out << sep;
			out << parameters[i] << " [" << units[i] << "]";
		}
		out << "\n";
		for (int idtime = 0; idtime < datas[0].size(); ++idtime) {
			if (idtime > 0)
				out << "\n";
			for (int idparam = 0; idparam < parameters.size(); ++idparam) {
				if (idparam > 0)
					out << sep;
				if (datas[idparam].size() > idtime)
					out << FDS(datas[idparam][idtime], 10, true);
			}
		}
	}
	
	String GetDLLPath() const		{return dllFile;}	
	static Function<bool(String, int, const Time &)> WhenWave;
	static Function<bool(String)> WhenPrint;
	static Time startCalc, lastLog, beginNoLicense;
	static int64 noLicenseTime;
	
private:
	Dl dll;
	String dllFile;
	
	HINSTANCE wave = 0, flex = 0;
	
	void (*CreateModel)(HINSTANCE *handle, HWND hCaller, int *status);
	void (*DestroyModel)(HINSTANCE handle, int *status);
	void (*LoadData)(HINSTANCE handle, LPCWSTR wcs, int *status);
	void (*SaveData)(HINSTANCE handle, LPCWSTR wcs, int *status);
	
	void (*CreateDiffraction)(HINSTANCE *handle, int *status);
	void (*DestroyDiffraction)(HINSTANCE handle, int *status);
	void (*LoadDiffractionData)(HINSTANCE handle, LPCWSTR wcs, int *status);
	void (*SaveDiffractionData)(HINSTANCE handle, LPCWSTR wcs, int *status);
	void (*CalculateDiffraction)(HINSTANCE handle, TProgressHandlerProc proc, int *status);
	void (*SaveDiffractionResults)(HINSTANCE handle, LPCWSTR wcs, int *status);
	void (*RunSimulation)(HINSTANCE handle, TSimulationHandlerProc proc, const TRunSimulationParameters *par, int *status);
	void (*CalculateStatics)(HINSTANCE handle, TProgressHandlerProc proc, int *status);
	void (*LoadSimulation)(HINSTANCE handle, LPCWSTR wcs, int *status);
	void (*SaveSimulation)(HINSTANCE handle, LPCWSTR wcs, int *status);
	void (*GetTimeHistory2)(HINSTANCE handle, void *nil, const TPeriod *period, int varID, double *lpValues, int *status);	
	void (*GetVarID)(HINSTANCE handle, LPCWSTR wcs, int *lpVarID, int *status);
	int  (*GetNumOfSamples)(HINSTANCE handle, const TPeriod *lpPeriod, int *status);
	void (*EnumerateObjects)(HINSTANCE handle, TEnumerateObjectsProc proc, int *lpNumOfObjects, int *status);
	void (*EnumerateVars2)(HINSTANCE handle, const TObjectExtra2 *lpObjectextra, int ResultType, TEnumerateVarsProc EnumerateVarsProc,
					int *lpNumberOfVars, int *status);
	void (*ObjectCalled)(HINSTANCE handle, LPCWSTR lpObjectName, TObjectInfo *lpObjectInfo, int *status);
	void (*CGetModelState)(HINSTANCE handle, int *lpModelState, int *status);
	
	void (*SetModelThreadCount)(HINSTANCE handle, int threadCount, int *status);
	int (*GetModelThreadCount)(HINSTANCE handle, int *status);
	
	void (*RegisterLicenceNotFoundHandler)(TLicenceNotFoundHandlerProc handler, int *lpStatus);
	
	int (*GetLastErrorString)(LPCWSTR wcs);
	
	void (*FinaliseLibrary)(int *status);
	
	String GetErrorString() {
		int len = GetLastErrorString(NULL);
		Buffer<wchar_t> rw(len);
		LPWSTR wcs = (LPWSTR)rw.begin();
		GetLastErrorString(wcs);
		return WideToString(wcs, len);
	}
	
	void throwError(String where, String errorString = Null) {
		String str = errorString;
		if (errorString == "")
			str = GetErrorString();
		throw Exc(Format("'%s': %s", where, str));
	}
};

#pragma pack(pop)

#endif