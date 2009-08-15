#ifndef CAMERA__HEADER
#define CAMERA__HEADER

#include <stdafx.h>

#include <Plane.h>


class Camera
{
protected:

	Vector3 EyePosition;
	Vector3 UpVector;
	Vector3 LookPosition;

    bool AllFacesDrawing;
	bool IsoMode;
	float IsoScalar;
	Uint16 MaxScalar;
	Uint16 MinScalar;

	bool SlidingMode;
	bool ZoomingMode;
	bool VerticalMode;

	CameraOrientation Orientation;

	Uint16 SliceA;
	Uint16 SliceB;
	Uint16 SliceTop;
	Uint16 SliceBottom;
	Uint16 ViewLevels;

	Uint8 LevelSeperation;

	float ViewWidth, ViewHight, ViewDepth;

	Plane FrustumPlanes[4];

    Vector3 Cursor;
    Vector3 MouseIntersection;

    float CursorLevel;

    Vector3 NearMouseClickPoint;
    Vector3 FarMouseClickPoint;

public:

	Camera();
	bool Init(bool IsoMetric);
	~Camera();

	float EyeX()    { return EyePosition.x; }
	float EyeY()    { return EyePosition.y; }
	float EyeZ()    { return EyePosition.z; }
	Vector3 Eye()   { return EyePosition; }

	float LookX()   { return LookPosition.x; }
	float LookY()   { return LookPosition.y; }
	float LookZ()   { return LookPosition.z; }
	Vector3 Look()  { return LookPosition; }

	//Uint8 getViewLevels()                       { return abs(SliceA - SliceB)/*ViewLevels*/; }
	//void setViewLevels(Uint8 NewValue);

	Uint8 getLevelSeperation()                  { return LevelSeperation; }
	void changeLevelSeperation(Sint8 Change);
	void setLevelSeperation(Sint8 NewValue);

    Uint16 getSliceTop()                        { return SliceTop; }
    Uint16 getSliceBottom()                        { return SliceBottom; }
    void SetSliceA(int newValue);
    void SetSliceB(int newValue);
    //void changeSliceTop(Sint16 Change);
    //void setSliceTop(Sint16 NewValue);

	CameraOrientation getOrientation()  { return Orientation; }
	void UpdateDirection();

	float getScalar()               { return IsoScalar; }

    bool InSlice(float Zlevel);
    float getShading(float Zlevel);

    void UnProjectPoint(int XPosition, int YPosition);

    Vector3 getNearMouseClickPoint()                    { return NearMouseClickPoint; }
    Vector3 setNearMouseClickPoint(Vector3 NewPoint)    { NearMouseClickPoint = NewPoint; return NewPoint; }

    Vector3 getFarMouseClickPoint()                     { return FarMouseClickPoint; }
    Vector3 setFarMouseClickPoint(Vector3 NewPoint)     { FarMouseClickPoint = NewPoint; return NewPoint; }

    Vector3 DetermineMouseIntersection(float MapZ);
    bool DetermineCursorIntersection();

    Vector3 getCursor()                     { return Cursor; }
    void setCursor(Vector3 NewPoint);

    Vector3 getMouseIntersection()          { return MouseIntersection; }

	bool isVerticalMode()                   { return VerticalMode;}
    void setVerticalMode(bool NewValue);

    Sint32 ZlevelSeperationAdjustment(Sint32);

	/**
	* This method is responsible for capturing our current view
	* frustum matrix and dumping the data into 4 planes that
	* we can query
	*/
	void generateViewFrustum();

	/**
	* This method is responsible for setting the perspective
	* matrix for our camera
	* @param fAspect - the aspect ratio of the view
	* @param z_min - the minimum z-plane
	* @param z_max - the maximum z-plane
	*/
	void setPerspectiveProj( float fAspect, float z_min, float z_max );

	/**
	* This method is responsible for setting the camera to Isometric/Orthographic
	* matrix for our camera
	* @param Width - the view windows Width
	* @param Hight - the view Windows Hight
	* @Param Depth - the view Windows Depth
	*/
	void setIsometricProj( float Width, float Hight, float Depth );

	/**
	* This method sets our camera viewpoint
	* @param vecEye - Vector3 of the eye point
	* @param vecLookAt - Vector3 coordinates of where we're looking
	* @param vecLookUp - Vector3 coordinates of the up direction
	*/
	void setViewMatrix( Vector3& vecEye, Vector3& vecLookAt, Vector3& vecUp);

	/**
	* This method rotates the view around a specified axis.
	* @param X - X axis rotation
	* @param Y - Y axis rotation
	* @param Z - Z axis rotation
	*/
	void RotateView(float X, float Y, float Z);

	/**
	* This method orbits the Camera around the LookPoint in only the X/Y plane.
	* @param Rotation - The Degrees of rotation, positive rotation is ?clock-wise.
	*/
	void OrbitView(float Rotation);

	/**
	* This method tilts the Camera by moving the Camera in the X/Y plane
	* either towards or away from the LookPosition.
	* @param Movement - Units of Camera movment, negative movment tilts the camera
	* to a top-down perspective, positive movment results in edge-on view
	* @param Min - Minimum allowed distance
	* @param Max - Maximum allowed distance
	*/
	void TiltView(float Movement, float Min, float Max);

	/**
	* This method zooms the Camera in and out by englargin or shinking the model
	* @param ZoomFactor - Greater then 1 zooms in, less then 1 zooms out
	*/
	void ZoomView(float ZoomFactor);

	/**
	* This method moves the Cameras Look Point in the X/Y Plane while maintaining
	* the same camera angle.
	* @param X - Movement in the X axis
	* @param Y - Movement in the Y axis
	*/
	void SlideView(float X, float Y);



	virtual void UpdateView();

	/**
	* This method can be used to update the view matrix based
	* on the mouse's motion (if need be)
	* @param pEvent - the SDL_Event structure containing the mouse events
	*/
	//virtual void onMouseEvent(SDL_Event* pEvent, Sint32 RelativeX, Sint32 RelativeY);
	void onMouseEvent(SDL_Event* Event, Sint32 RelativeX, Sint32 RelativeY, Uint8 *Keystate, Uint8 MouseButtonState, int RealX, int RealY);

	/**
	* This method every time events are polled to do the screen edge scroll
	* on the mouse's real position rather then relative position
	*/
    void onMousePoll();

	void MoveViewHorizontal(float X, float Y);

    void ConfineCursor();

    void ConfineLookPosition();


	void MoveViewVertical(float Z);

//    void setViewHight(Sint32 Change);

	/**
	* ChangeViewLevels modifies the number of z levels that will be rendered
	* @param Change - Amount of change, positive or negative
	*/
    void ChangeViewLevel(Sint32 Change);

	/**
	* SetDefaultView returns the camera to a looking at the Origin
	* looking northward with an Isometric angle
	*/
	void SetDefaultView();

	/**
	* CenterView returns the camera to at Center of the map
	* looking northward with an Isometric angle
	*/
    void CenterView(Vector3 CenterPoint);

    // get rid of?
	void PrintDebugging();

	/**
	* setCameraDirection rotates the camera to the desired cardinal
	* direction without moving it.
	* @param NewDirection - desired direction enum
	*/
	void setCameraOrientation(CameraOrientation NewOrientation);

    /**
	* sphereInFrustum is responsible for testing if a sphere is
	* within the frustrum.
	* @param Point - position of the sphere
	* @param float - radius of the sphere
	* @return bool - true if the sphere is inside the frustum
	*/
	bool sphereInFrustum(Vector3 Point, float Radius);


	void setAllFacesDrawing(bool NewValue);

	bool isAllFacesDrawing();
};

#endif // CAMERA__HEADER
