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

static float TKT_Dot(vector a, vector b) { return a[0]*b[0] + a[1]*b[1] + a[2]*b[2]; }

static bool TKT_ComputeCircleTangent(vector c1, float r1, bool ccw1,
                                     vector c2, float r2, bool ccw2,
                                     bool chooseUpper,
                                     out vector p1, out vector p2, out vector dir)
{
	// XZ only; keep incoming Y on centers
	vector u = c2 - c1; u[1] = 0;
	float D = Math.Sqrt(u[0]*u[0] + u[2]*u[2]);
	if (D < 0.001) return false;
	u = u / D;

	// 90° left of u
	vector v = Vector(-u[2], 0, u[0]);

	bool isExternal = (ccw1 == ccw2);
	float R; if (isExternal) R = (r1 - r2); else R = (r1 + r2);
	float cosA = Math.Clamp(R / D, -1.0, 1.0);
	float sinA = Math.Sqrt(Math.Max(0.0, 1.0 - cosA * cosA));
	float sgn; if (chooseUpper) sgn = 1.0; else sgn = -1.0;

	// radius direction at the tangent point on circle 1
	vector radial1 = u * cosA + v * (sgn * sinA);

	// for internal tangents, circle 2’s radial flips across v
	vector radial2; if (isExternal) radial2 = radial1; else radial2 = (u * cosA - v * (sgn * sinA));

	p1 = c1 + radial1 * r1;
	p2 = c2 + radial2 * r2;

	// tangent direction = -90° (right) from the radius
	dir = Vector(radial1[2], 0, -radial1[0]);
	// point from p1 toward p2
	if (TKT_Dot(dir, p2 - p1) < 0) dir = -dir;

	// keep heights
	p1[1] = c1[1]; p2[1] = c2[1];
	return true;
}