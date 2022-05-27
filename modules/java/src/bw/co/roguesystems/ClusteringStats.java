package bw.co.roguesystems;

public class ClusteringStats{	
	int count;
	StatsValues coreDistanceValues;
	StatsValues intraDistanceValues;
	
	public ClusteringStats(){
		count = 0;
		coreDistanceValues = new StatsValues();
		intraDistanceValues = new StatsValues();		
	}
	
	public void setCount(int count){
		this.count = count;
	}
	
	public void setCoreDistanceValues(int coreDistanceValues){
		this.coreDistanceValues = coreDistanceValues;
	}
	
	public void setIntraDistanceValues(int intraDistanceValues){
		this.intraDistanceValues = intraDistanceValues;
	}
	
	public int getCount(){
		return this.count;
	}
	
	public StatsValues getCoreDistanceValues(){
		return this.coreDistanceValues;
	}
	
	public StatsValues getIntraDistanceValues(){
		return this.intraDistanceValues;
	}
}
