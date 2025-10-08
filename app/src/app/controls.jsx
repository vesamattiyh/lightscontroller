import { FlatList, StyleSheet, Text, View } from 'react-native';
import RelayItem from '../components/relayitem';

const ControlsScreen = () => {
  return (
    <View style={styles.container}>
    <Text style={styles.title}>Lights Controller</Text>
    
    <FlatList style={styles.list}
        data={[
          {key: 'Toggle all'},
          {key: 'Reverse Light'},
          {key: 'Left Side Light'},
          {key: 'Right Side Light'},
        ]}
        renderItem={({item}) => <RelayItem>{item.key}</RelayItem>}
    />

    </View>
  );
};

const styles = StyleSheet.create({
  container: {
        flex: 1,
        justifyContent: 'center',
        alignItems: 'center',
        backgroundColor : '#000'
  },

  list : {
    paddingTop : 100
  },

  title : {
    position: 'absolute',
    top: 20,
    fontSize: 25,
    fontWeight: 'bold',
    color : '#FFF'
  },

  relayItem : {
    color : '#FFF'
  },
});

export default ControlsScreen;