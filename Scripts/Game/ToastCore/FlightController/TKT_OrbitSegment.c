class TKT_OrbitSegment : TKT_PathSegment
{
	vector m_center; float m_height; float m_radius; float m_speed; bool m_ccw; float m_yawDeg;
	float  m_loops; // <=0 => infinite, >0 => number of full turns

	protected const float TWO_PI = 6.283185307179586;
	protected float m_theta0;

	override float Duration()
	{
		if (m_loops <= 0) return 0; // infinite
		// time for loops turns: angle/omega; omega = v/r
		float omega = m_speed / Math.Max(0.01, m_radius);
		return (TWO_PI * m_loops) / omega;
	}
	
	void SetStartAtPoint(vector worldPoint)
	{
		vector center = m_center + Vector(0, m_height, 0);
		vector q = worldPoint - center;
	
		float yaw = m_yawDeg * 0.0174532925199433;
		float cy = Math.Cos(yaw), sy = Math.Sin(yaw);
	
		// rotate into local circle frame (undo yaw)
		float lx =  q[0]*cy + q[2]*sy;
		float lz = -q[0]*sy + q[2]*cy;
	
		m_theta0 = Math.Atan2(lz, lx); // -pi..pi
	}

	override bool Eval(float t, out vector pos, out vector vel, out vector fwd, out vector up)
	{
		float dir; if (m_ccw) dir = 1.0; else dir = -1.0;
		float theta = m_theta0 + (m_speed / Math.Max(0.01, m_radius)) * t * dir;
		
		float yaw = m_yawDeg * 0.0174532925199433;
		float cy = Math.Cos(yaw), sy = Math.Sin(yaw);
		float c = Math.Cos(theta), s = Math.Sin(theta);

		float x = m_radius * ( c*cy - s*sy );
		float z = m_radius * ( c*sy + s*cy );
		pos = m_center + Vector(0, m_height, 0) + Vector(x,0,z);

		float tx = (-s*cy - c*sy) * dir;
		float tz = (-s*sy + c*cy) * dir;
		fwd = _norm(Vector(tx,0,tz));

		vector right0 = _norm(_cross(Vector(0,1,0), fwd));
		vector up0    = _norm(_cross(fwd, right0));
		up = up0;
		vel = fwd * m_speed;
		
		if ((int)(t*10)%10==0) PrintFormat("[Orbit] t=%1", t);

		// stop when duration reached (for finite loops)
		float dur = Duration();
		if (dur > 0 && t >= dur) return false;
		return true; // keep going
	}
	
	override void DebugDraw(float t)
	{
		const int flags = ShapeFlags.ONCE | ShapeFlags.NOZBUFFER;
		const int colPath = 0xFF00FFFF;   // ARGB (A,R,G,B) cyan
		const int colCenter = 0xFFFFA000; // orange
		const int colTangent = 0xFFFF0000; // red
	
		// center + circle
		vector center = m_center + Vector(0, m_height, 0);
		Shape.CreateSphere(colCenter, flags, center, 0.5);
	
		const int N = 64;
		vector pts[N];
		float yaw = m_yawDeg * 0.0174532925199433;
		float cy = Math.Cos(yaw), sy = Math.Sin(yaw);
		for (int i=0; i<N; i++)
		{
			float a = (Math.PI2 * i) / (N-1); // 0..2π
			float c = Math.Cos(a);
			float s = Math.Sin(a);
			float x = m_radius * ( c*cy - s*sy );
			float z = m_radius * ( c*sy + s*cy );
			pts[i] = center + Vector(x, 0, z);
		}
		Shape.CreateLines(colPath, flags, pts, N);
	
		// current point + tangent (approx using t)
		vector pos, vel, fwd, up;
		if (Eval(t, pos, vel, fwd, up)) {
			Shape.CreateSphere(0xFFFFFFFF, flags, pos, 0.25);
			TKT_DrawLine(pos, pos + fwd * 3.0, 0xFFFF0000);
		}
	}
}

static float TKT_ScoreTangentAlign(vector center3D, bool ccw, vector pOnCircle, vector dir)
{
	vector radial = pOnCircle - center3D; radial[1] = 0;
	float L = Math.Sqrt(radial[0]*radial[0] + radial[2]*radial[2]);
	if (L > 0.0001) radial = radial / L;

	vector tA;
	if (ccw) tA = Vector(-radial[2], 0, radial[0]);   // +90°
	else     tA = Vector( radial[2], 0,-radial[0]);   // -90°

	return tA[0]*dir[0] + tA[2]*dir[2];
}