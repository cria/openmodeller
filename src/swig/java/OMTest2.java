/* 
 *
 *
 *
 */

import java.util.Hashtable;
import java.util.Map;

public class OMTest2 {

    public static void main(String argv[]) {
	int i;
	Hashtable h = new Hashtable();
	h.put("1", new Integer(10));
	h.put("3", new Integer(30));
	h.put("2", new Integer(20));
	h.put("4", new Integer(40));

	Object obj[] = h.entrySet().toArray();

	for (i = 0; i < obj.length; i++)
	    {
		Map.Entry entry = (Map.Entry) obj[i];
		System.out.println("Value[" + entry.getKey() + "]=" + 
				   entry.getValue());
	    }
    }
}
