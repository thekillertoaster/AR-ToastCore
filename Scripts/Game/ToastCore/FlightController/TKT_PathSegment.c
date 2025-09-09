class TKT_PathSegment {
	bool Eval(float t, out vector pos, out vector vel, out vector fwd, out vector up) { return false; }
	float Duration() { return 0; }
	
	void DebugDraw(float t) {}
	
	protected vector _cross(vector a, vector b) { return Vector(a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]); }
	protected vector _norm(vector v) 
	{
		float L=v.Length(); 
		if (L<0.0001)
			return "0 0 0"; 
		
		return v/L; 
	}
}

static void TKT_DrawLine(vector a, vector b, int col)
{
	const int flags = ShapeFlags.ONCE | ShapeFlags.NOZBUFFER;
	vector seg[2];
	seg[0] = a; seg[1] = b;
	Shape.CreateLines(col, flags, seg, 2);
}