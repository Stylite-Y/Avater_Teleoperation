/* LCM type definition class file
 * This file was automatically generated by lcm-gen
 * DO NOT MODIFY BY HAND!!!!
 */

package lcmtypes;
 
import java.io.*;
import java.util.*;
import lcm.lcm.*;
 
public final class command_phase_lcmt implements lcm.lcm.LCMEncodable
{
    public float phase[];
    public float command[];
 
    public command_phase_lcmt()
    {
        phase = new float[4];
        command = new float[3];
    }
 
    public static final long LCM_FINGERPRINT;
    public static final long LCM_FINGERPRINT_BASE = 0xec4884374c19cf77L;
 
    static {
        LCM_FINGERPRINT = _hashRecursive(new ArrayList<Class<?>>());
    }
 
    public static long _hashRecursive(ArrayList<Class<?>> classes)
    {
        if (classes.contains(lcmtypes.command_phase_lcmt.class))
            return 0L;
 
        classes.add(lcmtypes.command_phase_lcmt.class);
        long hash = LCM_FINGERPRINT_BASE
            ;
        classes.remove(classes.size() - 1);
        return (hash<<1) + ((hash>>63)&1);
    }
 
    public void encode(DataOutput outs) throws IOException
    {
        outs.writeLong(LCM_FINGERPRINT);
        _encodeRecursive(outs);
    }
 
    public void _encodeRecursive(DataOutput outs) throws IOException
    {
        for (int a = 0; a < 4; a++) {
            outs.writeFloat(this.phase[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeFloat(this.command[a]); 
        }
 
    }
 
    public command_phase_lcmt(byte[] data) throws IOException
    {
        this(new LCMDataInputStream(data));
    }
 
    public command_phase_lcmt(DataInput ins) throws IOException
    {
        if (ins.readLong() != LCM_FINGERPRINT)
            throw new IOException("LCM Decode error: bad fingerprint");
 
        _decodeRecursive(ins);
    }
 
    public static lcmtypes.command_phase_lcmt _decodeRecursiveFactory(DataInput ins) throws IOException
    {
        lcmtypes.command_phase_lcmt o = new lcmtypes.command_phase_lcmt();
        o._decodeRecursive(ins);
        return o;
    }
 
    public void _decodeRecursive(DataInput ins) throws IOException
    {
        this.phase = new float[(int) 4];
        for (int a = 0; a < 4; a++) {
            this.phase[a] = ins.readFloat();
        }
 
        this.command = new float[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.command[a] = ins.readFloat();
        }
 
    }
 
    public lcmtypes.command_phase_lcmt copy()
    {
        lcmtypes.command_phase_lcmt outobj = new lcmtypes.command_phase_lcmt();
        outobj.phase = new float[(int) 4];
        System.arraycopy(this.phase, 0, outobj.phase, 0, 4); 
        outobj.command = new float[(int) 3];
        System.arraycopy(this.command, 0, outobj.command, 0, 3); 
        return outobj;
    }
 
}

