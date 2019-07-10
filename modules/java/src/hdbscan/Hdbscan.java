/*
 * Hdbscan.java
 * 
 * Copyright 2018 Onalenna Junior Makhura
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
	  * @param minPoints
	  */
	private native void initHdbscan(int minPoints);
		
	/**
	 * 
	 * @param dataset
	 * @return
	 */
	private native int[] runImpl(double[][] dataset);

	/**
	 * 
	 * @param minPoints
	 * @return
	 */
	private native int[] reRunImpl(int minPoints);
	
	/**
	 * Call this method to clean up C allocated memory
	 * 
	 */ 
	public native void cleanHdbscan();
	
	/**
	 * 
	 * @param minPoints
	 */
	public Hdbscan(int minPoints){
		initHdbscan(minPoints);
	}
	
	/**
	 * 
	 * @param dataset
	 */
	public void run(double[][] dataset){
		labels = runImpl(dataset);
	}
	
	/**
	 * 
	 * @param minPts
	 */
	public void reRun(int minPts){
		labels = reRunImpl(minPts);
	}
	
	/**
	 * 
	 * @return
	 */
	public int[] getLabels(){
		return this.labels;
	}
	
	/**
	 * 
	 * @return
	 */
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
