package com.eucalyptus.sla;

import java.util.List;
import java.util.NoSuchElementException;
import com.eucalyptus.cluster.ClusterState;
import com.eucalyptus.cluster.Clusters;
import com.eucalyptus.cluster.Networks;
import com.eucalyptus.util.EucalyptusProperties;
import com.eucalyptus.util.NotEnoughResourcesAvailable;
import edu.ucsb.eucalyptus.cloud.Network;
import edu.ucsb.eucalyptus.cloud.NetworkToken;
import edu.ucsb.eucalyptus.cloud.ResourceToken;
import edu.ucsb.eucalyptus.cloud.VmAllocationInfo;
import org.apache.log4j.Logger;
import com.eucalyptus.util.EucalyptusProperties.TokenState;
import edu.ucsb.eucalyptus.msgs.EventRecord;

public class PrivateNetworkAllocator implements ResourceAllocator {
  private static Logger LOG = Logger.getLogger( PrivateNetworkAllocator.class );
  public void allocate( VmAllocationInfo vmInfo ) throws Exception {
    vmInfo.allocationTokens.each{ ResourceToken it ->
      if(vmInfo.networks.size() < 1) throw new NotEnoughResourcesAvailable( "At least one network group must be specified." );
      Network firstNet = vmInfo.networks.first( );
      try {
        firstNet = Networks.getInstance( ).lookup( firstNet.name );
      } catch ( NoSuchElementException e ) {
        Networks.getInstance( ).registerIfAbsent( firstNet, Networks.State.ACTIVE );
        firstNet = Networks.getInstance( ).lookup( firstNet.name );
      }
      ClusterState clusterState = Clusters.getInstance( ).lookup( it.cluster ).state;
      NetworkToken networkToken = clusterState.getNetworkAllocation( vmInfo.request.userId, network.name );
      LOG.info( EventRecord.create( SubnetIndexAllocator.class, TokenState.accepted, networkToken.toString( ) ) );
      it.networkTokens += networkToken;
    }
  }
  
  public void fail( VmAllocationInfo vmInfo, Throwable t ) {
    Network firstNet = vmInfo.networks.first();
    vmInfo.getAllocationTokens( ).each{ it -> 
      it.getPrimaryNetwork( )?.getIndexes( ).each{
        firstNet?.returnNetworkIndex( i );
      }
    }
    
  }
}  