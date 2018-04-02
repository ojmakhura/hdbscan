package hdbscan;

import java.util.HashMap;
import java.util.ArrayList;

public class Hdbscan{
	static {
		System.loadLibrary("hdbscan_jni");
	};

	private int[] labels;
	
	/**
	 * Initialise hdbscan*
	 * 
	 */ 
	private native void initHdbscan(int minPoints);
	
	/**
	 * 
	 *
	 */  
	private native int[] runImpl(double[][] dataset);
	
	/**
	 * 
	 * 
	 */ 
	private native int[] reRunImpl(int minPoints);
	
	/**
	 * Call this method to clean up C allocated memory
	 * 
	 */ 
	public native void cleanHdbscan();
		
	public Hdbscan(int minPoints){
		initHdbscan(minPoints);
	}
	
	public void run(double[][] dataset){
		labels = runImpl(dataset);
	}
	
	public void reRun(int minPts){
		labels = reRunImpl(minPts);
	}
	
	public int[] getLabels(){
		return this.labels;
	}
	
	public HashMap<Integer, ArrayList<Integer>> getClusterMap(){
		HashMap<Integer, ArrayList<Integer>> table = new HashMap<Integer, ArrayList<Integer>>();
		
		if(labels == null || labels.length == 0){
			return null;
		}
		
		for(int i = 0; i < labels.length; i++){
			Integer key = labels[i];
			ArrayList<Integer> list = table.get(key);
			
			if(list == null){
				list = new ArrayList<Integer>();
				list.add(i);
				table.put(key, list);
			} else{
				
				list.add(i);
			}
		}
		
		return table;
	}
		
	public static void main(String[] args){
		
		double mydata[][] = {	{1,1,3,3},
								{1,2,8,18},
								{2,1,8,18},
								{2,2,3,5},
								{2,4,5,3},
								{3,1,3,4},
								{3,2,9,18},
								{3,3,9,19},
								{3,4,9,20},
								{3,5,4,3},
								{3,6,20,2},
								{3,7,10,44},
								{4,2,4,5},
								{4,3,3,5},
								{4,5,10,18},
								{4,6,10,18},
								{5,1,3,4},
								{5,4,5,5},
								{6,0,4,4},
								{6,1,4,4},
								{6,2,8,19},
								{6,3,8,18},
								{6,4,1,20}
							};
		Hdbscan sc = new Hdbscan(3);
		sc.run(mydata);
		int[] labels = sc.getLabels();
		System.out.println("labels = [");
		for(int i = 0; i < labels.length; i++){
			System.out.print(labels[i] + ", ");
		}
		System.out.println("]");
	}
}
