class TKT_LineSegment : TKT_PathSegment {
	vector m_a; vector m_b; float m_speed;
	float m_len;
	void Init() { m_len = (m_b - m_a).Length(); }

	override float Duration() { return m_len / Math.Max(0.01, m_speed); }

	override bool Eval(float t, out vector pos, out vector vel, out vector fwd, out vector up) {
		float T = Math.Clamp(t / Duration(), 0.0, 1.0);
		pos = m_a + (m_b - m_a) * T;
		fwd = _norm(m_b - m_a);
		up  = Vector(0,1,0);
		vel = fwd * m_speed;
		PrintFormat("[Line] t=%1", t);
		return T < 1.0;
	}
	
	override void DebugDraw(float t)
	{
		const int flags = ShapeFlags.ONCE | ShapeFlags.NOZBUFFER;
		const int col = 0xFF00FF00; // green
		TKT_DrawLine(m_a, m_b, 0xFF00FF00);
	
		// moving point
		vector pos, vel, fwd, up;
		if (Eval(t, pos, vel, fwd, up)) {
			Shape.CreateSphere(0xFFFFFFFF, flags, pos, 0.25);
			TKT_DrawLine(pos, pos + fwd * 3.0, 0xFFFF0000);
		}
	}
}