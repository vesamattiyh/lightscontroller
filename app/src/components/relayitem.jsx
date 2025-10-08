import { useState } from 'react';
import { StyleSheet, Text, TouchableOpacity, View } from 'react-native';

const RelayItem = ({children}) => {
  
    const [state, setState] = useState(false);

    const onPressAction = () => {
        console.log("Button pressed");
        setState(!state);
    }

    return (
    <View style={styles.box}>
      <Text style={styles.txt}>{children}</Text>
      <TouchableOpacity 
        style={{ ...styles.btn, backgroundColor: state ? "green" : "orange" }}
        onPress={onPressAction}>
        <Text style={styles.txt}>{state ? 'On' : 'Off'}</Text>
      </TouchableOpacity>
    </View>
  )
}

export default RelayItem

const styles = StyleSheet.create({

    box : {
        width: '70vw',
        flexDirection: 'row',
        justifyContent:'space-between',
        alignContent: 'center',
        alignItems: 'center',
        border: 'solid',
        color:'#FFF',
        borderRadius: 5,
        margin: 5,
        padding: 10
    },

    txt : {
        color : '#FFF',
        marginLeft : 5,
        textAlign:'center',
    },

    btn : {
        width: '30%',
        marginRight : 5,
        borderRadius: 5,
    }
})