
import java.util.ArrayList;
import java.util.HashMap;

import bw.co.roguesystems.hdbscan.Hdbscan;
public class Htest {
	public static double[][] readInDataSet(String fileName, String delimiter) throws IOException {
		BufferedReader reader = new BufferedReader(new FileReader(fileName));
		ArrayList<double[]> dataSet = new ArrayList<double[]>();
		int numAttributes = -1;
		int lineIndex = 0;
		String line = reader.readLine();

		while (line != null) {
			line = line.trim();
			lineIndex++;
			String[] lineContents = line.split(delimiter);
			//System.out.println("Line " + lineIndex + " has " + lineContents.length);
			if (numAttributes == -1)
				numAttributes = lineContents.length;
			else if (lineContents.length != numAttributes)
				System.err.println("Line " + lineIndex + " of data set has incorrect number of attributes.");

			double[] attributes = new double[numAttributes];
			for (int i = 0; i < numAttributes; i++) {
				try {
					// If an exception occurs, the attribute will remain 0:
					attributes[i] = Double.parseDouble(lineContents[i].trim());
				} catch (NumberFormatException nfe) {
					System.err.println("Illegal value on line " + lineIndex + " of data set: " + lineContents[i]);
				}
			}

			dataSet.add(attributes);
			line = reader.readLine();
		}

		reader.close();
		double[][] finalDataSet = new double[dataSet.size()][numAttributes];

		for (int i = 0; i < dataSet.size(); i++) {
			finalDataSet[i] = dataSet.get(i);
		}

		return finalDataSet;
	}

	public static void main(String[] args) {

		double mydata[][] = readInDataSet("example_data_set.csv", ",");
		Hdbscan sc = new Hdbscan(3);
		sc.run(mydata);
		int[] labels = sc.getLabels();
		System.out.print("labels = [");
		for(int i = 0; i < labels.length; i++){
			System.out.print(labels[i] + ", ");
		}
		System.out.println("]");
		
		HashMap<Integer, ArrayList<Integer>> ds = sc.getClusterMap();
		
		for(Integer in : ds.keySet()) {
			System.out.print(in + " : ");
			ArrayList<Integer> idx = ds.get(in);
			for(int i = 0; i < idx.size(); i++) {
				System.out.print(idx.get(i) + ", ");
			}
			System.out.println();
		}
	}

}
