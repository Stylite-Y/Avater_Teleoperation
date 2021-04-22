/* LCM type definition class file
 * This file was automatically generated by lcm-gen
 * DO NOT MODIFY BY HAND!!!!
 */

package lcmtypes;
 
import java.io.*;
import java.util.*;
import lcm.lcm.*;
 
public final class core_t implements lcm.lcm.LCMEncodable
{
    public double phase[];
    public double cmd[];
    public double q[];
    public double dq[];
    public double z;
    public double v[];
    public double z_axis[];
    public double omega[];
 
    public core_t()
    {
        phase = new double[2];
        cmd = new double[3];
        q = new double[12];
        dq = new double[12];
        v = new double[3];
        z_axis = new double[3];
        omega = new double[3];
    }
 
    public static final long LCM_FINGERPRINT;
    public static final long LCM_FINGERPRINT_BASE = 0x774778e6685ecce8L;
 
    static {
        LCM_FINGERPRINT = _hashRecursive(new ArrayList<Class<?>>());
    }
 
    public static long _hashRecursive(ArrayList<Class<?>> classes)
    {
        if (classes.contains(lcmtypes.core_t.class))
            return 0L;
 
        classes.add(lcmtypes.core_t.class);
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
        for (int a = 0; a < 2; a++) {
            outs.writeDouble(this.phase[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeDouble(this.cmd[a]); 
        }
 
        for (int a = 0; a < 12; a++) {
            outs.writeDouble(this.q[a]); 
        }
 
        for (int a = 0; a < 12; a++) {
            outs.writeDouble(this.dq[a]); 
        }
 
        outs.writeDouble(this.z); 
 
        for (int a = 0; a < 3; a++) {
            outs.writeDouble(this.v[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeDouble(this.z_axis[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeDouble(this.omega[a]); 
        }
 
    }
 
    public core_t(byte[] data) throws IOException
    {
        this(new LCMDataInputStream(data));
    }
 
    public core_t(DataInput ins) throws IOException
    {
        if (ins.readLong() != LCM_FINGERPRINT)
            throw new IOException("LCM Decode error: bad fingerprint");
 
        _decodeRecursive(ins);
    }
 
    public static lcmtypes.core_t _decodeRecursiveFactory(DataInput ins) throws IOException
    {
        lcmtypes.core_t o = new lcmtypes.core_t();
        o._decodeRecursive(ins);
        return o;
    }
 
    public void _decodeRecursive(DataInput ins) throws IOException
    {
        this.phase = new double[(int) 2];
        for (int a = 0; a < 2; a++) {
            this.phase[a] = ins.readDouble();
        }
 
        this.cmd = new double[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.cmd[a] = ins.readDouble();
        }
 
        this.q = new double[(int) 12];
        for (int a = 0; a < 12; a++) {
            this.q[a] = ins.readDouble();
        }
 
        this.dq = new double[(int) 12];
        for (int a = 0; a < 12; a++) {
            this.dq[a] = ins.readDouble();
        }
 
        this.z = ins.readDouble();
 
        this.v = new double[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.v[a] = ins.readDouble();
        }
 
        this.z_axis = new double[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.z_axis[a] = ins.readDouble();
        }
 
        this.omega = new double[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.omega[a] = ins.readDouble();
        }
 
    }
 
    public lcmtypes.core_t copy()
    {
        lcmtypes.core_t outobj = new lcmtypes.core_t();
        outobj.phase = new double[(int) 2];
        System.arraycopy(this.phase, 0, outobj.phase, 0, 2); 
        outobj.cmd = new double[(int) 3];
        System.arraycopy(this.cmd, 0, outobj.cmd, 0, 3); 
        outobj.q = new double[(int) 12];
        System.arraycopy(this.q, 0, outobj.q, 0, 12); 
        outobj.dq = new double[(int) 12];
        System.arraycopy(this.dq, 0, outobj.dq, 0, 12); 
        outobj.z = this.z;
 
        outobj.v = new double[(int) 3];
        System.arraycopy(this.v, 0, outobj.v, 0, 3); 
        outobj.z_axis = new double[(int) 3];
        System.arraycopy(this.z_axis, 0, outobj.z_axis, 0, 3); 
        outobj.omega = new double[(int) 3];
        System.arraycopy(this.omega, 0, outobj.omega, 0, 3); 
        return outobj;
    }
 
}
