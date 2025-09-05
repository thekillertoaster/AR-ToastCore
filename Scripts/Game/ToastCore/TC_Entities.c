class TC_Entities
{
	// --- Proxy Functions (From other Classes but are commonly used within this context)
	static RplComponent Proxy_EntRpl (IEntity targetEntity)
	{
		return TC_Replication.EntRpl(targetEntity);
	}
	
	static EntitySpawnParams EntitySpawnParamsForPosition(vector position)
	{
		EntitySpawnParams p = new EntitySpawnParams();
		p.TransformMode = ETransformMode.WORLD;
		Math3D.MatrixIdentity4(p.Transform);
		p.Transform[3] = position;
		return p;
	}
	
	// Replicated delete
	static bool RplDelete(IEntity ent, bool releaseFromReplication = true)
	{
		if (!ent) return false;

		RplComponent rpl = Proxy_EntRpl(ent);
		if (rpl)
			RplComponent.DeleteRplEntity(ent, releaseFromReplication);
		else 
		{
			Print("[TC_Entities] SafeDelete: Could not find replication component");
			return null;
		}

		return true;
	}

	// Spawn a prefab at a world-space position. By default, only spawns on authority to ensure replication.
	static IEntity SpawnAtPosition(ResourceName prefabToSpawn, vector positionToSpawnAt, bool enforceAuthority = true)
	{
		if (!prefabToSpawn)
		{
			Print("[TC_Entities] SpawnAtPosition: null prefab name");
			return null;
		}

		if (enforceAuthority && !TC_Replication.Master())
		{
			PrintFormat("[TC_Entities] Not master; refusing to spawn %1 at %2", prefabToSpawn, positionToSpawnAt);
			return null;
		}

		Resource res = Resource.Load(prefabToSpawn);
		if (!res)
		{
			PrintFormat("[TC_Entities] Failed to load resource: %1", prefabToSpawn);
			return null;
		}

		EntitySpawnParams p = EntitySpawnParamsForPosition(positionToSpawnAt);

		IEntity spawned = GetGame().SpawnEntityPrefab(res, GetGame().GetWorld(), p);
		if (!spawned)
		{
			PrintFormat("[TC_Entities] SpawnEntityPrefab returned null for %1", prefabToSpawn);
			return null;
		}

		// Sanity warning: if the prefab has no RplComponent it won't replicate to clients.
		RplComponent rpl = Proxy_EntRpl(spawned);
		if (!rpl)
			PrintFormat("[TC_Entities] WARNING: spawned %1 has no RplComponent; it will be local-only", prefabToSpawn);

		return spawned;
	}
	
	static float GetSpeed(IEntity ent) 
	{
		Physics phys = ent.GetPhysics();
		
		if (!phys) return -1;
		return phys.GetVelocity().Length();
	}
}
