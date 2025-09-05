class TC_Replication 
{
	// Are we the authority that should spawn/despawn replicated entities?
	// If you have a context entity, pass it so we can use its RplComponent; otherwise fall back to server check.
	static bool Master(IEntity context = null)
	{
		if (context)
		{
			RplComponent rpl = RplComponent.Cast(context.FindComponent(RplComponent));
			if (rpl) return rpl.IsMaster();
		}
		return Replication.IsServer();
	}
	
	static RplComponent EntRpl(IEntity targetEntity)
	{
		RplComponent rpl = RplComponent.Cast(targetEntity.FindComponent(RplComponent));
		if (rpl)
			return rpl;
		else
			return null;
	}
}