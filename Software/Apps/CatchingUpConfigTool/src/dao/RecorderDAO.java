package dao;

import java.util.List;

import dm.Recorder;

public interface RecorderDAO {
	/**
	 * Return Recorder where Recorder.id = id
	 * @param id
	 * @return
	 */
	public Recorder getRecorder(int id);
	
	/**
	 * Return Recorder list
	 * @return
	 */
	public List<Recorder> getRecorderList();

	public boolean createRecorder ( Recorder rec );
	
	public boolean updateRecorder ( Recorder rec );
	
	public boolean parringRecorder ( Recorder rec );
}
