package hdbscan;

import java.util.HashMap;
import java.util.ArrayList;

public class Hdbscan{
	static {
		System.loadLibrary("hdbscan_java");
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
	
}
